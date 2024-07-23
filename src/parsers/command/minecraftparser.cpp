#include "minecraftparser.h"

#include "re2c_generated_functions.h"

#include "nlohmann/json.hpp"
#include "uberswitch.hpp"

using json = nlohmann::json;

namespace Command {
    MinecraftParser::MinecraftParser() {
        /*
           connect(this, &QObject::destroyed, [ = ](QObject *obj) {
           qDebug() << "MinecraftParser emit QObject destroyed:" << obj;
           });
         */
    }

    QString MinecraftParser::eatListSep(QChar sepChr, QChar endChr) {
        const int start = pos();

        while (curChar().isSpace()) {
            advance();
        }
        if (curChar() != endChr) {
            expect(sepChr,
                   "Unexpected %1, expecting separator %2 or closing character");
            advance();
            while (curChar().isSpace()) {
                advance();
            }
        }
        return spanText(start);
    }

    QSharedPointer<AngleNode> MinecraftParser::parseAxis(
        AxisParseOptions options, bool &isLocal) {
        using AxisType = AngleNode::AxisType;
        static const char *mixCoordErrMsg(QT_TR_NOOP(
                                              "Cannot mix world & local coordinates (everything must either use ^ or not)."));

        const bool canBeLocal = options & AxisParseOption::CanBeLocal;
        const bool onlyInt    = options & AxisParseOption::OnlyInteger;
        const bool isFirst    = options & AxisParseOption::FirstAxis;
        bool       isValid    = true;

        const int start     = pos();
        bool      hasPrefix = false;

        const auto axisVal =
            [&](AngleNode *axis) {
                bool        ok;
                QStringView literal;
                if (onlyInt) {
                    auto [raw, value] = parseInteger(ok);
                    literal           = raw;
                    axis->setValue(value);
                } else {
                    auto [raw, value] = parseFloat(ok);
                    literal           = raw;
                    axis->setValue(value);
                }
                if (isValid) {
                    isValid |= ok;
                }
                if (!ok) {
                    reportError(QT_TR_NOOP("Invalid axis value"),
                                {}, start, literal.length());
                }
            };

        AnglePtr axis;
        if (curChar() == '~') {
            axis      = AnglePtr::create(AxisType::Relative);
            hasPrefix = true;
            if (isFirst || !isLocal) {
                isLocal = false;
            } else {
                isValid = false;
                reportError(mixCoordErrMsg);
            }
            advance();
        } else if (curChar() == '^') {
            axis      = AnglePtr::create(AxisType::Local);
            hasPrefix = true;
            if (!canBeLocal) {
                isValid = false;
                reportError(QT_TR_NOOP(
                                "Local coordinates can't be used here"));
            } else if (isFirst) {
                isLocal = true;
            } else if (!isLocal) {
                isValid = false;
                reportError(mixCoordErrMsg);
            }
            advance();
        } else {
            axis = AnglePtr::create(AxisType::Absolute);
        }
        if (hasPrefix) {
            if ((!curChar().isNull()) && (curChar() != ' ')) {
                axisVal(axis.get());
            }
        } else if (curChar() == ' ') {
            isValid = false;
            reportError(QT_TR_NOOP("Missing an axis here"), {}, start, 1);
        } else {
            axisVal(axis.get());
        }

        axis->setText(spanText(start));
        axis->setIsValid(isValid);
        return axis;
    }

    void MinecraftParser::parseAxes(ArgumentNode *node,
                                    AxisParseOptions options) {
        using PT = ArgumentNode::ParserType;
        bool      isLocal = false;
        const int start   = pos();

        /*axes->setX(parseAxis(options, isLocal)); */
        switch (node->parserType()) {
            case PT::ColumnPos:
            case PT::Rotation:
            case PT::Vec2: {
                auto *axes = static_cast<TwoAxesNode *>(node);
//                axes->setFirstAxis(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setFirstAxis(
                    parseAxis(options | AxisParseOption::FirstAxis, isLocal));
                axes->firstAxis()->setTrailingTrivia(eat(' ',
                                                         "Unexpected %1, expecting %2 to separate between axes"));
//                axes->setSecondAxis(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setSecondAxis(parseAxis(options, isLocal));
                break;
            }

            case PT::BlockPos:
            case PT::Vec3: {
                auto *axes = static_cast<XyzNode *>(node);
//                axes->setX(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setX(parseAxis(options | AxisParseOption::FirstAxis,
                                     isLocal));
                axes->x()->setTrailingTrivia(eat(' ',
                                                 "Unexpected %1, expecting %2 to separate between axes"));
//                axes->setY(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setY(parseAxis(options, isLocal));
                axes->y()->setTrailingTrivia(eat(' ',
                                                 "Unexpected %1, expecting %2 to separate between axes"));
//                axes->setZ(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setZ(parseAxis(options, isLocal));
                break;
            }

            default:
                break;
        }

        node->setLength(pos() - start);
    }

    QSharedPointer<NbtCompoundNode> MinecraftParser::
    parseCompoundTag() {
        return parseMap<NbtCompoundNode, NbtNode>('{', '}', ':',
                                                  [this](const QString &) {
            return parseTagValue();
        }, true);
    }

    QSharedPointer<NbtNode> MinecraftParser::parseTagValue() {
        const int start = pos();

        switch (curChar().toLatin1()) {
            case '{': {
                return parseCompoundTag();
            }

            case '[': {
                advance();
                if (peekNext(1) == ';') {
                    switch (curChar().toLower().toLatin1()) {
                        case 'b': {
                            return parseArrayTag<NbtByteArrayNode, NbtByteNode>(
                                "Invalid SNBT tag in a byte array tag");
                        }

                        case 'i': {
                            return parseArrayTag<NbtIntArrayNode, NbtIntNode>(
                                "Invalid SNBT tag in an integer array tag");
                        }

                        case 'l': {
                            return parseArrayTag<NbtLongArrayNode, NbtLongNode>(
                                "Invalid SNBT tag in a long array tag");
                        }

                        default: {
                            return parseListTag();
                        }
                    }
                } else {
                    return parseListTag();
                }
            }

            case '"':
            case '\'': {
                const QString &&quoted = getQuotedString();
                return QSharedPointer<NbtStringNode>::create(
                    spanText(start), quoted, true);
            }

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
            case '.': {
                return parseNumericTag();
            }

            case 't': {
                if (peek(4) == "true"_QL1) {
                    advance(4);
                    return QSharedPointer<NbtByteNode>::create(
                        spanText(start), true, true);
                }
            }

            case 'f': {
                if (peek(5) == "false"_QL1) {
                    advance(5);
                    return QSharedPointer<NbtByteNode>::create(
                        spanText(start), false, true);
                }
            }

            default: {
                const auto value = getLiteralString();
                if (value.isEmpty()) {
                    reportError(QT_TR_NOOP("Invalid empty tag value"));
                } else {
                    return QSharedPointer<NbtStringNode>::create(
                        spanText(value), true);
                }
            }
        }
        return QSharedPointer<NbtStringNode>::create(QString(), false);
    }

    QSharedPointer<NbtNode> MinecraftParser::parseNumericTag() {
        const int start = pos();

        const QStringView literal = advanceView(re2c::snbtNumber(peekRest()));
        bool              ok      = false;

        switch (curChar().toLower().toLatin1()) {
            case 'b': {
                advance();
//                const short int value = literal.toShort(&ok);
                const int8_t value = strWithExpToDec<int8_t>(literal, ok);
                if (ok) {
                    return QSharedPointer<NbtByteNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT byte tag"),
                               { literal.toString() }, start, literal.length());
                    break;
                }
            }

            case 'd': {
                advance();
                const double value = literal.toDouble(&ok);
                if (ok || std::isinf(value)) {
                    return QSharedPointer<NbtDoubleNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT double tag"),
                               { literal.toString() }, start, literal.length());
                    break;
                }
            }

            case 'f': {
                advance();
                const double value = literal.toFloat(&ok);
                if (ok || std::isinf(value)) {
                    return QSharedPointer<NbtFloatNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT float tag"),
                               { literal.toString() }, start, literal.length());
                    break;
                }
            }

            case 'l': {
                advance();
                const long long value = strWithExpToDec<long long>(literal, ok);
                if (ok) {
                    return QSharedPointer<NbtLongNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT long tag"),
                               { literal.toString() }, start, literal.length());
                    break;
                }
            }

            case 's': {
                advance();
                const short int value = strWithExpToDec<short>(literal, ok);
                if (ok) {
                    return QSharedPointer<NbtShortNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT short tag"),
                               { literal.toString() }, start, literal.length());
                    break;
                }
            }

            default: {
                if (literal.contains('.')) {
                    const double value = literal.toDouble(&ok);
                    if (ok || std::isinf(value)) {
                        return QSharedPointer<NbtDoubleNode>::create(
                            spanText(start), value, true);
                    } else {
                        throwError(QT_TR_NOOP(
                                       "%1 is not a vaild SNBT double tag"),
                                   { literal.toString() },
                                   start,
                                   literal.length());
                        break;
                    }
                } else {
//                    const int value = literal.toInt(&ok);
                    const int value = strWithExpToDec<int>(literal, ok);
                    if (ok) {
                        return QSharedPointer<NbtIntNode>::create(
                            spanText(start), value, true);
                    } else {
                        throwError(
                            QT_TR_NOOP(
                                "%1 is not a vaild SNBT integer tag"),
                            { literal.toString() }, start, literal.length());
                    }
                }
            }
        }
        return nullptr;
    }

    QSharedPointer<NbtListNode> MinecraftParser::parseListTag() {
        const int    start = pos() - 1;
        const auto &&ret   = QSharedPointer<NbtListNode>::create(0);

        Q_ASSERT(ret != nullptr);

        ret->setLeftText(spanText(start));
        bool first = true;

        while (curChar() != ']') {
            const QString &&trivia = skipWs(false);
            const auto    &&elem   = parseTagValue();
            if (!trivia.isEmpty()) {
                elem->setLeadingTrivia(trivia);
            }
            if (first || (elem->tagType() == ret->prefix())) {
                first = false;
            } else {
//                reportError(QT_TR_NOOP(
//                                "Type of elements in this list tag must be the same"));
            }
            elem->setTrailingTrivia(eatListSep(',', ']'));
            ret->append(elem);
        }
        ret->setRightText(eat(']'));
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<MapNode> MinecraftParser::
    parseEntityAdvancements() {
        return parseMap<MapNode, ParseNode>('{', '}', '=',
                                            [this](const QString &) -> NodePtr {
            if (curChar() == '{') {
                return parseMap<MapNode, BoolNode>(
                    '{', '}', '=', [this](const QString &) ->
                    QSharedPointer<BoolNode> {
                    return brigadier_bool();
                }, false, R"(a-zA-z0-9-_:.+/)"_QL1);
            } else {
                return brigadier_bool();
            }
        }, false, R"(a-zA-z0-9-_:.+/)"_QL1);
    }

    QSharedPointer<MapNode> MinecraftParser::parseEntityArguments() {
        return parseMap<MapNode, ParseNode>(
            '[', ']', '=', [this](
                const QString &key) -> QSharedPointer<ParseNode> {
            uswitch (key) {
                ucase ("x"_QL1):
                ucase ("y"_QL1):
                ucase ("z"_QL1):
                ucase ("dx"_QL1):
                ucase ("dy"_QL1):
                ucase ("dz"_QL1): {
                    return brigadier_double();
                }
                ucase ("distance"_QL1):
                ucase ("x_rotation"_QL1):
                ucase ("y_rotation"_QL1): {
                    return minecraft_floatRange();
                }

                ucase ("level"_QL1): {
                    return minecraft_intRange();
                }
                ucase ("limit"_QL1): {
                    return brigadier_integer();
                }
                ucase ("predicate"_QL1): {
                    const auto &&ret = parseInvertible();
                    ret->setNode(minecraft_resourceLocation());
                    return ret;
                }
                ucase ("sort"_QL1): {
                    constexpr static std::array options {
                        "nearest"_QL1, "furthest"_QL1, "random"_QL1,
                        "arbitrary"_QL1 };
                    const QString &&literal = oneOf(options);
                    return QSharedPointer<StringNode>::create(
                        spanText(literal), !literal.isEmpty());
                }
                ucase ("gamemode"_QL1): {
                    const auto &&ret        = parseInvertible();
                    const QString &&literal =
                        oneOf(staticSuggestions<GamemodeNode>);
                    ret->setNode(QSharedPointer<StringNode>::create(
                                     spanText(literal), !literal.isEmpty()));
                    return ret;
                }
                ucase ("name"_QL1): {
                    const int start  = pos();
                    const auto &&ret = parseInvertible();
                    QString literal;
                    if (curChar() == '"' || curChar() == '\'') {
                        literal = getQuotedString();
                    } else {
                        literal = getLiteralString().toString();
                    }
                    ret->setNode(
                        QSharedPointer<StringNode>::create(
                            spanText(start), literal, errorIfNot(
                                !literal.isEmpty(),
                                QT_TR_NOOP(
                                    "Invalid empty name in target selector"))));
                    return ret;
                }
                ucase ("type"_QL1): {
                    const auto &&ret    = parseInvertible();
                    const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(
                        0);

                    parseResourceLocation(resLoc.get(), true);
                    ret->setNode(resLoc);
                    return ret;
                }
                ucase ("nbt"_QL1): {
                    const auto &&ret = parseInvertible();
                    ret->setNode(parseCompoundTag());
                    return ret;
                }
                ucase ("tag"_QL1):
                ucase ("team"_QL1): {
                    const auto &&ret    = parseInvertible();
                    QStringView literal = getLiteralString();
                    ret->setNode(QSharedPointer<StringNode>::create(
                                     spanText(literal), true));
                    return ret;
                }
                ucase ("scores"_QL1): {
                    return parseMap<MapNode, IntRangeNode>(
                        '{', '}', '=', [this](const QString &) {
                        return minecraft_intRange();
                    });
                }
                ucase ("advancements"): {
                    return parseEntityAdvancements();
                }
            }
            throwError(QT_TR_NOOP("Unknown entity argument name: %1"), { key });
        });
    }

    QSharedPointer<TargetSelectorNode> MinecraftParser::
    parseTargetSelector() {
        const auto &&ret   = QSharedPointer<TargetSelectorNode>::create(0);
        const int    start = pos();

        eat('@');
        using Variable = TargetSelectorNode::Variable;
        switch (curChar().toLatin1()) {
            case 'a': {
                ret->setVariable(Variable::A);
                break;
            }

            case 'e': {
                ret->setVariable(Variable::E);
                break;
            }

            case 'n': {
                ret->setVariable(Variable::N);
                if (gameVer < QVersionNumber(1, 21)) {
                    reportError(
                        QT_TR_NOOP(
                            "The @n selector is only available on 1.21+"),
                        {},
                        pos() - 1,
                        2);
                }
                break;
            }

            case 'p': {
                ret->setVariable(Variable::P);
                break;
            }

            case 'r': {
                ret->setVariable(Variable::R);
                break;
            }

            case 's': {
                ret->setVariable(Variable::S);
                break;
            }

            default: {
                reportError(QT_TR_NOOP("Invalid target selector variable: %1"),
                            { QStringLiteral("@%1").arg(curChar()) });
            }
        }
        advance();
        ret->setLeftText(spanText(start));
        if (curChar() == '[')
            ret->setArgs(parseEntityArguments());
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<NbtPathStepNode> MinecraftParser::parseNbtPathStep() {
        using StepType = NbtPathStepNode::Type;
        const auto &&ret   = QSharedPointer<NbtPathStepNode>::create(0);
        const int    start = pos();

        switch (curChar().toLatin1()) {
            case '"': {
                ret->setName(QSharedPointer<StringNode>::create(
                                 spanText(start), getQuotedString(), true));
                if (curChar() == '{')
                    ret->setFilter(parseCompoundTag());
                break;
            }

            case '{': {
                ret->setType(StepType::Root);
                ret->setFilter(parseCompoundTag());
                break;
            }

            case '[': {
                ret->setType(StepType::Index);
                ret->setIsValid(true);
                const int leftPos = pos();
                advance();
                const QString &&trivia = skipWs(false);
                if (curChar() == ']') {
                    /* Selects all. Continues. */
                    ret->setLeftText(spanText(leftPos));
                } else {
                    ret->setLeftText(spanText(QStringLiteral("[")));
                    if (curChar() == '{') {
                        ret->setFilter(parseCompoundTag());
                        ret->filter()->setLeadingTrivia(trivia);
                        ret->filter()->setTrailingTrivia(skipWs());
                    } else {
                        ret->setIndex(brigadier_integer());
                        ret->index()->setLeadingTrivia(trivia);
                        ret->index()->setTrailingTrivia(skipWs());
                    }
                }
                ret->setRightText(eat(']'));
                break;
            }

            case '\'': {
                if (gameVer >= QVersionNumber(1, 20)) {
                    ret->setName(QSharedPointer<StringNode>::create(
                                     spanText(start), getQuotedString(), true));
                    if (curChar() == '{')
                        ret->setFilter(parseCompoundTag());
                    break;
                } else {
                    [[fallthrough]];
                }
            }

            default: {
                const auto name = advanceView(re2c::nbtPathKey(peekRest()));
                ret->setName(
                    QSharedPointer<StringNode>::create(
                        spanText(name),
                        errorIfNot(!name.isEmpty(),
                                   QT_TR_NOOP("Invalid empty NBT path key"))));
                if (curChar() == '{')
                    ret->setFilter(parseCompoundTag());
            };
        }
        /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
        if (curChar() == '.') {
            advance();
            ret->setTrailingTrivia(spanText(pos() - 1));
        }
        /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
        ret->setLength(pos() - start);
        /*qDebug() << ret->toString() << ret->index(); */
        return ret;
    }

    QSharedPointer<ParticleColorNode> MinecraftParser::
    parseParticleColor() {
        const int start = pos();
        auto    &&color = QSharedPointer<ParticleColorNode>::create(0);

        color->setR(brigadier_float());
        color->r()->setTrailingTrivia(eat(' ',
                                          "Unexpected %1, expecting %2 to separate between color values"));
        color->setG(brigadier_float());
        color->g()->setTrailingTrivia(eat(' ',
                                          "Unexpected %1, expecting %2 to separate between color values"));
        color->setB(brigadier_float());
        color->setLength(pos() - start);
        return color;
    }

    QSharedPointer<ParticleNode> MinecraftParser::parseOldParticle() {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<ParticleNode>::create(0);
        const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(0);

        parseResourceLocation(resLoc.get());

        QString fullId;
        if (!resLoc->nspace()) {
            fullId = resLoc->id()->text();
        } else {
            const QString &&nspace = resLoc->nspace()->text();
            if (nspace.isEmpty() || (nspace == "minecraft"_QL1)) {
                fullId = resLoc->id()->text();
            } else {
                fullId = nspace + ":"_QL1 + resLoc->id()->text();
            }
        }
        ret->setResLoc(std::move(resLoc));

        uswitch (fullId) {
            ucase ("block"_QL1):
            ucase ("block_marker"_QL1):
            ucase ("falling_dust"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                ret->setParams(minecraft_blockState());
                break;
            }
            ucase ("dust"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                const auto &&color = parseParticleColor();
                color->setTrailingTrivia(eat(' '));
                ret->setParams(std::move(color), brigadier_float());
                break;
            }
            ucase ("dust_color_transition"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                const auto &&startColor = parseParticleColor();
                startColor->setTrailingTrivia(eat(' '));
                const auto &&size = brigadier_float();
                size->setTrailingTrivia(eat(' '));
                const auto &&endColor = parseParticleColor();
                ret->setParams(std::move(startColor), std::move(size),
                               std::move(endColor));
                break;
            }
            ucase ("item"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                ret->setParams(minecraft_itemStack());
                break;
            }
            ucase ("sculk_charge"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                ret->setParams(brigadier_float());
                break;
            }
            ucase ("shriek"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                ret->setParams(brigadier_integer());
                break;
            }
            ucase ("vibration"_QL1): {
                resLoc->setTrailingTrivia(eat(' ',
                                              "Unexpected %1, expecting %2 to separate between particle and parameters"));
                const auto &&pos = minecraft_vec3();
                pos->setTrailingTrivia(eat(' '));
                ret->setParams(std::move(pos), brigadier_integer());
                break;
            }
        }

        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<InvertibleNode> MinecraftParser::parseInvertible() {
        const bool   isNegative = curChar() == '!';
        const auto &&ret        =
            QSharedPointer<InvertibleNode>::create(isNegative);

        if (isNegative) {
            advance();
            ret->setLeftText(spanText(pos() - 1));
        }
        return ret;
    }

    QSharedPointer<ListNode> MinecraftParser::parseItemPredEntry() {
        auto    &&obj   = QSharedPointer<ListNode>::create(0);
        const int start = pos();

        while (curChar() != ',' && curChar() != ']') {
            const auto &&trivia = skipWs(false);
            const auto &&elem   = parseItemPredMatch();
            elem->setLeadingTrivia(trivia);
            obj->append(elem);

            const int trailingTriviaPos = pos();
            while (curChar().isSpace()) {
                advance();
            }
            if (curChar() != ',' && curChar() != ']') {
                expect('|',
                       "Unexpected %1, expecting separator %2 or closing character");
                advance();
                while (curChar().isSpace()) {
                    advance();
                }
            }

            elem->setTrailingTrivia(spanText(trailingTriviaPos));
        }
        obj->setLength(pos() - start);
        return obj;
    }

    QSharedPointer<ItemPredicateMatchNode> MinecraftParser::parseItemPredMatch(
        const bool isPredicate) {
        const int start = pos();

        const auto &&trivia = skipWs(false);
        auto       &&key    = parseInvertible();

        key->setLeadingTrivia(trivia);
        key->setNode(minecraft_resourceLocation());

        const auto &&trailingTriviaPos = pos();
        skipWs(false);

        auto &&obj =
            QSharedPointer<ItemPredicateMatchNode>::create(key, isPredicate);

        bool      hasValue    = false;
        const int sepValuePos = pos();
        if (curChar() == '=') {
            hasValue = true;
            advance();
            obj->first->setTrailingTrivia(spanText(trailingTriviaPos));
            obj->setMode(ItemPredicateMatchNode::Mode::FullMatch);
        } else if (curChar() == '~') {
            hasValue = true;
            advance();
            obj->first->setTrailingTrivia(spanText(trailingTriviaPos));
            obj->setMode(ItemPredicateMatchNode::Mode::PartialMatch);
        } else {
            obj->first->setTrailingTrivia(spanText(trailingTriviaPos));
        }

        if (hasValue) {
            const auto &&valueTrivia = skipWs(false);
            const auto &&value       = parseTagValue();
            value->setLeadingTrivia(spanText(valueTrivia));
            value->setTrailingTrivia(skipWs(false));
            obj->setIsValid(obj->isValid() && value->isValid());
            obj->second = std::move(value);
        }
        if (!isPredicate &&
            obj->mode() == ItemPredicateMatchNode::Mode::PartialMatch) {
            reportError(
                QT_TR_NOOP(
                    "Cannot test for sub-predicate in a item stack"),
                {}, sepValuePos, pos() - sepValuePos);
        }


        obj->setLength(pos() - start);

        if (!isPredicate) {
            if (key->isInverted()) {
                if (hasValue) {
                    reportError(
                        QT_TR_NOOP(
                            "Cannot invert a key-value pair of component in an item stack"),
                        {},
                        start,
                        obj->length());
                } else if (gameVer < QVersionNumber(1, 21)) {
                    reportError(
                        QT_TR_NOOP(
                            "Removal of default components in an item stack is only available on 1.21+"),
                        {},
                        start,
                        obj->length());
                }
            } else if (!hasValue) {
                reportError(QT_TR_NOOP("Missing component value"),
                            {}, start, obj->length());
            }
        }
        return obj;
    }

    void MinecraftParser::parseResourceLocation(ResourceLocationNode *node,
                                                bool acceptTag) {
//        const static QRegularExpression charsetRegex{ QStringLiteral(
//                                                          "[0-9a-z-_\\/.]*") };

        bool      hasError = false;
        const int start    = pos();
        SpanPtr   nspace;
        SpanPtr   id;

        if (curChar() == '#') {
            node->setIsTag(true);
            if (!acceptTag) {
                reportError(QT_TR_NOOP(
                                "A resource location tag is not allowed here."));
                hasError = true;
            }
            advance();
            node->setLeftText(spanText(start));
        }

        const auto idStrView = advanceView(re2c::resLocPart(peekRest()));
        id = SpanPtr::create(spanText(idStrView));
        if (curChar() == ':') {
            advance();
            nspace = std::move(id);
            id     = SpanPtr::create(
                spanText(advanceView(re2c::resLocPart(peekRest()))));
            id->setLeadingTrivia(spanText(QStringLiteral(":")));
        }
        if (nspace) {
            if (nspace->text().contains('/')) {
                reportError(
                    QT_TR_NOOP(
                        "The character '/' is not allowed in the namespace"),
                    {}, start, nspace->length());
                hasError = true;
            } else {
                nspace->setIsValid(true);
            }
        }
        Q_ASSERT(id != nullptr);
        if (id->leftText().isNull() && id->text().isEmpty()) {
            reportError(QT_TR_NOOP("Invalid empty namespaced ID"));
            hasError = true;
        } else {
            id->setIsValid(true);
        }

        if (nspace) {
            node->setNamespace(std::move(nspace));
        }
        node->setId(std::move(id));

        node->setLength(pos() - start);
        node->setIsValid(!hasError);
    }

    void MinecraftParser::parseBlock(BlockStateNode *node, bool acceptTag) {
        const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(0);

        parseResourceLocation(resLoc.get(), acceptTag);
        node->setResLoc(std::move(resLoc));

        if (curChar() == '[') {
            node->setStates(
                parseMap<MapNode, ParseNode>('[', ']', '=',
                                             [this](const QString &) {
                const static QVariantMap props{ { "type", "word" } };
                return brigadier_string(props);
            }));
        }
        if (curChar() == '{')
            node->setNbt(parseCompoundTag());
    }

    void Command::MinecraftParser::parseEntity(Command::EntityNode *node,
                                               bool allowFakePlayer) {
        const int curPos = pos();

        switch (curChar().toLower().toLatin1()) {
            case '@': {
                node->setNode(parseTargetSelector());
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f': {
                auto &&uuid = minecraft_uuid();
                if (uuid->isValid()) {
                    node->setNode(std::move(uuid));
                    break;
                } else {
                    setPos(curPos);
                    m_errors.pop_back();
                    [[fallthrough]];
                }
            }
            default: {
                const auto literal = allowFakePlayer
                    ? getUntil(QChar::Space)
                    : advanceView(re2c::realPlayerName(peekRest()));

                node->setNode(
                    QSharedPointer<StringNode>::create(
                        spanText(literal),
                        errorIfNot(!literal.isEmpty(),
                                   QT_TR_NOOP("Invalid empty player name"))));
            }
        }
    }

    void MinecraftParser::parseInlinableResource(InlinableResourceNode *node) {
        if (curChar() == '{') {
            node->setNode(parseCompoundTag());
        } else {
            node->setNode(minecraft_resourceLocation());
        }
    }

    NodePtr MinecraftParser::invokeMethod(ArgumentNode::ParserType parserType,
                                          const QVariantMap &props) {
        using ParserType = ArgumentNode::ParserType;
        if ((int)parserType < (int)ParserType::Angle) {
            return SchemaParser::invokeMethod(parserType, props);
        }
        switch (parserType) {
            case ParserType::Angle: { return minecraft_angle(); }
            case ParserType::BlockState: { return minecraft_blockState(); }
            case ParserType::BlockPos: { return minecraft_blockPos(); }
            case ParserType::BlockPredicate: { return minecraft_blockPredicate();
            }
            case ParserType::Color: { return minecraft_color(); }
            case ParserType::ColumnPos: { return minecraft_columnPos(); }
            case ParserType::Component: { return minecraft_component(); }
            case ParserType::Dimension: { return minecraft_dimension(); }
            case ParserType::Entity: { return minecraft_entity(props); }
            case ParserType::EntityAnchor: { return minecraft_entityAnchor(); }
            case ParserType::EntitySummon: { return minecraft_entitySummon(); }
            case ParserType::FloatRange: { return minecraft_floatRange(props); }
            case ParserType::Function: { return minecraft_function(); }
            case ParserType::Gamemode: { return minecraft_gamemode(); }
            case ParserType::GameProfile: { return minecraft_gameProfile(); }
            case ParserType::Heightmap: { return minecraft_heightmap(); }
            case ParserType::IntRange: { return minecraft_intRange(props); }
            case ParserType::ItemEnchantment: { return minecraft_itemEnchantment();
            }
            case ParserType::ItemPredicate: { return minecraft_itemPredicate();
            }
            case ParserType::ItemSlot: { return minecraft_itemSlot(); }
            case ParserType::ItemSlots: { return minecraft_itemSlots(); }
            case ParserType::ItemStack: { return minecraft_itemStack(); }
            case ParserType::LootModifier: { return minecraft_lootModifier(); }
            case ParserType::LootPredicate: { return minecraft_lootPredicate();
            }
            case ParserType::LootTable: { return minecraft_lootTable(); }
            case ParserType::Message: { return minecraft_message(); }
            case ParserType::MobEffect: { return minecraft_mobEffect(); }
            case ParserType::NbtCompoundTag: { return minecraft_nbtCompoundTag();
            }
            case ParserType::NbtPath: { return minecraft_nbtPath(); }
            case ParserType::NbtTag: { return minecraft_nbtTag(); }
            case ParserType::Objective: { return minecraft_objective(); }
            case ParserType::ObjectiveCriteria:  {
                return minecraft_objectiveCriteria();
            }
            case ParserType::Operation: { return minecraft_operation(); }
            case ParserType::Particle: { return minecraft_particle(); }
            case ParserType::Resource: { return minecraft_resource(); }
            case ParserType::ResourceKey: { return minecraft_resourceKey(); }
            case ParserType::ResourceOrTag: {
                return minecraft_resourceOrTag();
            }
            case ParserType::ResourceOrTagKey: {
                return minecraft_resourceOrTagKey();
            }
            case ParserType::ResourceLocation: {
                return minecraft_resourceLocation();
            }
            case ParserType::Rotation: { return minecraft_rotation(); }
            case ParserType::ScoreHolder: { return minecraft_scoreHolder(props);
            }
            case ParserType::ScoreboardSlot: {
                return minecraft_scoreboardSlot();
            }
            case ParserType::Style: { return minecraft_style(); }
            case ParserType::Swizzle: { return minecraft_swizzle(); }
            case ParserType::Team: { return minecraft_team(); }
            case ParserType::TemplateMirror: { return minecraft_templateMirror();
            }
            case ParserType::TemplateRotation: { return
                                                     minecraft_templateRotation();
            }
            case ParserType::Time: { return minecraft_time(); }
            case ParserType::Uuid: { return minecraft_uuid(); }
            case ParserType::Vec2: { return minecraft_vec2(); }
            case ParserType::Vec3: { return minecraft_vec3(); }
            case ParserType::InternalGreedyString: {
                return parseGreedyString();
            }
            case ParserType::InternalRegexPattern: {
                return parseRegexPattern();
            }

            default: {
                Q_UNREACHABLE();
                return nullptr;
            }
        }
    }

    QSharedPointer<AngleNode> MinecraftParser::minecraft_angle() {
        bool _ = false;

        return parseAxis(AxisParseOption::NoOption, _);
    }

    QSharedPointer<BlockPosNode> MinecraftParser::
    minecraft_blockPos() {
        const auto &&ret = QSharedPointer<BlockPosNode>::create(0);

        parseAxes(ret.get(), AxisParseOption::CanBeLocal);
        return ret;
    }

    QSharedPointer<BlockStateNode> MinecraftParser::
    minecraft_blockState() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<BlockStateNode>::create(0);

        parseBlock(ret.get(), false);
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<BlockPredicateNode> MinecraftParser::
    minecraft_blockPredicate() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<BlockPredicateNode>::create(0);

        parseBlock(ret.get(), true);
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ColorNode> MinecraftParser::minecraft_color() {
        const QString &&literal = oneOf(staticSuggestions<ColorNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<ColorNode>::create(spanText(literal), true);
        } else {
            return QSharedPointer<ColorNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<ColumnPosNode> MinecraftParser::
    minecraft_columnPos() {
        const auto &&ret = QSharedPointer<ColumnPosNode>::create(0);

        parseAxes(ret.get(), AxisParseOption::OnlyInteger |
                  AxisParseOption::CanBeLocal);
        return ret;
    }

    QSharedPointer<ComponentNode> MinecraftParser::
    minecraft_component() {
        const int    curPos = pos();
        const auto &&rest   = getRest();

        try {
            json       &&j   = json::parse(rest.toString().toStdString());
            const auto &&ret =
                QSharedPointer<ComponentNode>::create(spanText(rest));
            ret->setValue(std::move(j));
            return ret;
        }  catch (const json::parse_error &err) {
            /* TODO: Process JSON errors for localization */
            reportError(err.what(), {}, curPos + err.byte - 1);
            return QSharedPointer<ComponentNode>::create(spanText(rest));
        }
    }

    QSharedPointer<DimensionNode> MinecraftParser::
    minecraft_dimension() {
        const auto &&ret = QSharedPointer<DimensionNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<EntityNode> MinecraftParser::minecraft_entity(
        const QVariantMap &props) {
        const auto &&ret = QSharedPointer<EntityNode>::create(0);

        ret->setPlayerOnly(props[QStringLiteral(
                                     "type")].toString() == "players"_QL1);
        ret->setSingleOnly(props[QStringLiteral(
                                     "amount")].toString() == "single"_QL1);

        parseEntity(ret.get(), false);
        return ret;
    }

    QSharedPointer<EntityAnchorNode> MinecraftParser::
    minecraft_entityAnchor() {
        const QString &&literal = oneOf(staticSuggestions<EntityAnchorNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<EntityAnchorNode>::create(spanText(literal),
                                                            true);
        } else {
            return QSharedPointer<EntityAnchorNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<EntitySummonNode> MinecraftParser::
    minecraft_entitySummon() {
        const auto &&ret = QSharedPointer<EntitySummonNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<FloatRangeNode> MinecraftParser::
    minecraft_floatRange(const QVariantMap &props) {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<FloatRangeNode>::create(0);
        bool         hasMax = false;

        if (peek(2) == QLatin1String("..")) {
            hasMax = true;
            advance(2);
        }

        const auto &&num1 = brigadier_float(props);
        if (!hasMax) {
            bool hasDoubleDot = false;
            if (peek(2) == ".."_QL1) {
                advance(2);
                hasDoubleDot = true;
            } else if (!num1->text().isEmpty() && num1->text().back() == '.'_QL1
                       && curChar() == '.'_QL1) {
                advance();
                num1->chopTrailingDot();
                hasDoubleDot = true;
            }
            if (hasDoubleDot) {
                if (curChar().isDigit() || curChar() == '.' ||
                    curChar() == '-') { // "min..max"
                    ret->setMaxValue(brigadier_float(props), true);
                }
                num1->setTrailingTrivia(spanText(QStringLiteral("..")));
                ret->setMinValue(std::move(num1), true); // "min.."
            } else {
                ret->setExactValue(std::move(num1));     // "value"
            }
        } else {                                         // "..max"
            num1->setLeadingTrivia(spanText(QStringLiteral("..")));
            ret->setMaxValue(std::move(num1), false);
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<FunctionNode> MinecraftParser::
    minecraft_function() {
        const auto &&ret = QSharedPointer<FunctionNode>::create(0);

        parseResourceLocation(ret.get(), true);
        return ret;
    }

    QSharedPointer<GamemodeNode> MinecraftParser::minecraft_gamemode() {
        using GameMode = GamemodeNode::Mode;
        const QString &&literal = oneOf(staticSuggestions<GamemodeNode>);
        GameMode        mode    = GamemodeNode::Mode::Unknown;

        if (!literal.isEmpty()) {
            uswitch (literal) {
                ucase ("creative"): {
                    mode = GameMode::Creative;
                    break;
                }
                ucase ("survival"): {
                    mode = GameMode::Survival;
                    break;
                }
                ucase ("adventure"): {
                    mode = GameMode::Adventure;
                    break;
                }
                ucase ("spectator"): {
                    mode = GameMode::Spectator;
                    break;
                }
            }

            return QSharedPointer<GamemodeNode>::create(spanText(literal),
                                                        mode, true);
        } else {
            return QSharedPointer<GamemodeNode>::create(
                spanText(getUntil(QChar::Space)), mode, false);
        }
    }

    QSharedPointer<GameProfileNode> MinecraftParser::minecraft_gameProfile() {
        const auto &&ret = QSharedPointer<GameProfileNode>::create(0);

        ret->setPlayerOnly(true);

        parseEntity(ret.get(), false);
        return ret;
    }

    QSharedPointer<HeightmapNode> MinecraftParser::minecraft_heightmap() {
        const QString &&literal = oneOf(staticSuggestions<HeightmapNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<HeightmapNode>::create(spanText(literal),
                                                         true);
        } else {
            return QSharedPointer<HeightmapNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<IntRangeNode> MinecraftParser::
    minecraft_intRange(const QVariantMap &props) {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<IntRangeNode>::create(0);
        bool         hasMax = false;

        if (peek(2) == ".."_QL1) {
            hasMax = true;
            advance(2);
        }
        const auto &&num1 = brigadier_integer(props);
        if (!hasMax) {
            if (peek(2) == ".."_QL1) {
                advance(2);
                if (curChar().isDigit() || curChar() == '-') { // "min..max"
                    ret->setMaxValue(brigadier_integer(props), false);
                }
                num1->setTrailingTrivia(spanText(QStringLiteral("..")));
                ret->setMinValue(std::move(num1), true); // "min.."
            } else {
                ret->setExactValue(std::move(num1));     // "value"
            }
        } else {                                         // "..max"
            num1->setLeadingTrivia(spanText(QStringLiteral("..")));
            ret->setMaxValue(std::move(num1), false);
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ItemEnchantmentNode> MinecraftParser::
    minecraft_itemEnchantment() {
        const auto &&ret = QSharedPointer<ItemEnchantmentNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<ItemSlotNode> MinecraftParser::minecraft_itemSlot() {
        const auto slot = advanceView(re2c::itemSlot(peekRest()));

        return QSharedPointer<ItemSlotNode>::create(
            spanText(slot), errorIfNot(!slot.isEmpty(),
                                       QT_TR_NOOP("Invalid empty item slot")));
    }

    QSharedPointer<ItemSlotsNode> MinecraftParser::minecraft_itemSlots() {
        const auto slot = advanceView(re2c::itemSlots(peekRest()));

        return QSharedPointer<ItemSlotsNode>::create(
            spanText(slot),
            errorIfNot(!slot.isEmpty(),
                       QT_TR_NOOP("Invalid empty item slot or slot range")));
    }

    QSharedPointer<ItemStackNode> MinecraftParser::minecraft_itemStack() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<ItemStackNode>::create(0);

        ret->setResLoc(minecraft_resourceLocation());

        if (curChar() == '[' && gameVer >= QVersionNumber(1, 20, 5)) {
            ret->setComponents(
                parseList<ListNode, ItemPredicateMatchNode>('[', ']', ',',
                                                            [this]() {
                return parseItemPredMatch(false);
            }));
        }

        if (curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ItemPredicateNode> MinecraftParser::
    minecraft_itemPredicate() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<ItemPredicateNode>::create(0);

        const bool from_1_20_5 = gameVer >= QVersionNumber(1, 20, 5);

        if (curChar() == '*' && from_1_20_5) {
            ret->setAll(true);
            advance();
        } else {
            const auto &&resLoc =
                QSharedPointer<ResourceLocationNode>::create(0);

            parseResourceLocation(resLoc.get(), true);
            ret->setResLoc(std::move(resLoc));
        }

        if (curChar() == '[' && from_1_20_5) {
            ret->setComponents(
                parseList<ListNode, ListNode>('[', ']', ',', [this]() {
                return parseItemPredEntry();
            }));
        }

        if (curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<LootModifierNode> MinecraftParser::minecraft_lootModifier() {
        const auto &&ret = QSharedPointer<LootModifierNode>::create(0);

        parseInlinableResource(ret.get());
        return ret;
    }

    QSharedPointer<LootPredicateNode> MinecraftParser::minecraft_lootPredicate()
    {
        const auto &&ret = QSharedPointer<LootPredicateNode>::create(0);

        parseInlinableResource(ret.get());
        return ret;
    }

    QSharedPointer<LootTableNode> MinecraftParser::minecraft_lootTable() {
        const auto &&ret = QSharedPointer<LootTableNode>::create(0);

        parseInlinableResource(ret.get());
        return ret;
    }

    QSharedPointer<MessageNode> MinecraftParser::minecraft_message() {
        return QSharedPointer<MessageNode>::create(spanText(getRest()), true);
    }

    QSharedPointer<MobEffectNode> MinecraftParser::
    minecraft_mobEffect() {
        const auto &&ret = QSharedPointer<MobEffectNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<NbtCompoundNode> MinecraftParser::
    minecraft_nbtCompoundTag() {
        return parseCompoundTag();
    }

    QSharedPointer<NbtPathNode> MinecraftParser::
    minecraft_nbtPath() {
        const auto &&ret   = QSharedPointer<NbtPathNode>::create(0);
        const int    start = pos();

        ret->append(parseNbtPathStep());
        auto last = ret->last();
        while (last->trailingTrivia() == '.' || curChar() == '[' ||
               curChar() == '"' ||
               ((gameVer >= QVersionNumber(1, 20)) && (curChar() == '\''))) {
            const auto &&step = parseNbtPathStep();

            if ((step->type() == NbtPathStepNode::Type::Key)
                && (!(last->trailingTrivia() == '.'))) {
                reportError(QT_TR_NOOP(
                                "Missing character '.' before a named tag"),
                            {}, start + last->length() - 1);
            }
            ret->append(step);
            last = ret->last();
            /*qDebug() << curChar() << curChar().isNull() << last->hasTrailingDot(); */
            if (curChar().isNull())
                break;
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<NbtNode> MinecraftParser::minecraft_nbtTag() {
        return parseTagValue();
    }

    QSharedPointer<ObjectiveNode> MinecraftParser::
    minecraft_objective() {
        const int   curPos  = pos();
        QStringView objname = advanceView(re2c::objective(peekRest()));
        bool        valid   = true;

        if (objname.isEmpty()) {
            reportError(QT_TR_NOOP("Invalid empty objective"));
            valid = false;
        } else if ((objname.length() > 16) &&
                   (gameVer < QVersionNumber(1, 18, 2))) {
            reportError(QT_TR_NOOP(
                            "Objective '%1' must be less than 16 characters"),
                        { objname.toString() }, curPos, objname.length());
            valid = false;
        }
        return QSharedPointer<ObjectiveNode>::create(spanText(objname), valid);
    }

    QSharedPointer<ObjectiveCriteriaNode> MinecraftParser::
    minecraft_objectiveCriteria() {
        const auto criteria = advanceView(re2c::objectiveCriteria(peekRest()));

        return QSharedPointer<ObjectiveCriteriaNode>::create(
            spanText(criteria),
            errorIfNot(!criteria.isEmpty(),
                       QT_TR_NOOP("Invalid empty objective criteria")));
    }

    QSharedPointer<OperationNode> MinecraftParser::
    minecraft_operation() {
        const QString &&literal = oneOf(staticSuggestions<OperationNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<OperationNode>::create(spanText(literal),
                                                         true);
        } else {
            return QSharedPointer<OperationNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<ParticleNode> MinecraftParser::minecraft_particle() {
        if (gameVer >= QVersionNumber(1, 20, 5)) {
            const int    start = pos();
            const auto &&ret   = QSharedPointer<ParticleNode>::create(0);

            ret->setResLoc(minecraft_resourceLocation());

            if (curChar() == '{') {
                ret->setOptions(parseCompoundTag());
            }

            ret->setLength(pos() - start);
            return ret;
        } else {
            return parseOldParticle();
        }
    }

    QSharedPointer<ResourceNode> MinecraftParser::
    minecraft_resource() {
        const auto &&ret = QSharedPointer<ResourceNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<ResourceKeyNode> MinecraftParser::minecraft_resourceKey() {
        const auto &&ret = QSharedPointer<ResourceKeyNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<ResourceOrTagNode> MinecraftParser::
    minecraft_resourceOrTag() {
        const auto &&ret = QSharedPointer<ResourceOrTagNode>::create(0);

        parseResourceLocation(ret.get(), true);
        return ret;
    }

    QSharedPointer<ResourceOrTagKeyNode> MinecraftParser::
    minecraft_resourceOrTagKey() {
        const auto &&ret = QSharedPointer<ResourceOrTagKeyNode>::create(0);

        parseResourceLocation(ret.get(), true);
        return ret;
    }

    QSharedPointer<ResourceLocationNode> MinecraftParser::
    minecraft_resourceLocation() {
        const auto &&ret = QSharedPointer<ResourceLocationNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<RotationNode> MinecraftParser::
    minecraft_rotation() {
        const auto &&ret = QSharedPointer<RotationNode>::create(0);

        parseAxes(ret.get(), AxisParseOption::NoOption);
        return ret;
    }

    QSharedPointer<ScoreHolderNode> MinecraftParser::
    minecraft_scoreHolder(const QVariantMap &props) {
        if (curChar() == '*') {
            auto &&ret = QSharedPointer<ScoreHolderNode>::create(1);
            ret->setSingleOnly(props[QStringLiteral(
                                         "amount")].toString() == "single"_QL1);
            ret->setAll(true);
            advance();
            return ret;
        } else {
            const auto &&ret = QSharedPointer<ScoreHolderNode>::create(0);
            ret->setSingleOnly(props[QStringLiteral(
                                         "amount")].toString() == "single"_QL1);
            parseEntity(ret.get(), true);
            return ret;
        }
    }

    QSharedPointer<ScoreboardSlotNode> MinecraftParser::
    minecraft_scoreboardSlot() {
        QString slot;

        if (gameVer >= QVersionNumber(1, 20, 2)) {
            slot = oneOf(staticSuggestions_ScoreboardSlotNode_v1_20_2);
        } else {
            slot = oneOf(staticSuggestions<ScoreboardSlotNode>);
        }

        if (!slot.isEmpty()) {
            return QSharedPointer<ScoreboardSlotNode>::create(spanText(slot),
                                                              true);
        } else {
            return QSharedPointer<ScoreboardSlotNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<StyleNode> MinecraftParser::minecraft_style() {
        const int    curPos = pos();
        const auto &&rest   = getRest();

        try {
            json       &&j   = json::parse(rest.toString().toStdString());
            const auto &&ret =
                QSharedPointer<StyleNode>::create(spanText(rest));
            ret->setValue(std::move(j));
            return ret;
        }  catch (const json::parse_error &err) {
            reportError(err.what(), {}, curPos + err.byte - 1);
            return QSharedPointer<StyleNode>::create(spanText(rest));
        }
    }

    QSharedPointer<SwizzleNode> MinecraftParser::
    minecraft_swizzle() {
        const int         start = pos();
        SwizzleNode::Axes axes;
        QString           acceptedChars = QStringLiteral("xyz");

        while (acceptedChars.contains(curChar())) {
            switch (curChar().toLatin1()) {
                case 'x': {
                    axes |= SwizzleNode::Axis::X;
                    break;
                }

                case 'y': {
                    axes |= SwizzleNode::Axis::Y;
                    break;
                }

                case 'z': {
                    axes |= SwizzleNode::Axis::Z;
                    break;
                }
            }
            acceptedChars = acceptedChars.replace(curChar(), QString());
            advance();
        }
        return QSharedPointer<SwizzleNode>::create(spanText(start), axes);
    }

    QSharedPointer<TeamNode> MinecraftParser::minecraft_team() {
        const auto literal = getLiteralString();

        return QSharedPointer<TeamNode>::create(
            spanText(literal),
            errorIfNot(!literal.isEmpty(), QT_TR_NOOP("Invalid empty team")));
    }

    QSharedPointer<TimeNode> MinecraftParser::minecraft_time() {
        const int curPos = pos();
        auto      unit   = TimeNode::Unit::ImplicitTick;
        bool      ok;

        auto [raw, value] = parseFloat(ok);
        if (!ok) {
            reportError(QT_TR_NOOP("Invalid time"), {}, curPos, raw.length());
        }

        switch (curChar().toLatin1()) {
            case 'd': {
                unit = TimeNode::Unit::Day;
                advance();
                break;
            }

            case 's': {
                unit = TimeNode::Unit::Second;
                advance();
                break;
            }

            case 't': {
                unit = TimeNode::Unit::Tick;
                advance();
                break;
            }
        }

        auto &&ret = QSharedPointer<TimeNode>::create(spanText(curPos),
                                                      value, unit);
        ret->setIsValid(ok);
        return ret;
    }

    QSharedPointer<TemplateMirrorNode> MinecraftParser::minecraft_templateMirror()
    {
        const QString &&literal = oneOf(staticSuggestions<TemplateMirrorNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<TemplateMirrorNode>::create(spanText(literal),
                                                              true);
        } else {
            return QSharedPointer<TemplateMirrorNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<TemplateRotationNode> MinecraftParser::
    minecraft_templateRotation() {
        const QString &&literal =
            oneOf(staticSuggestions<TemplateRotationNode>);

        if (!literal.isEmpty()) {
            return QSharedPointer<TemplateRotationNode>::create(
                spanText(literal), true);
        } else {
            return QSharedPointer<TemplateRotationNode>::create(
                spanText(getUntil(QChar::Space)), false);
        }
    }

    QSharedPointer<UuidNode> MinecraftParser::minecraft_uuid() {
        QUuid uuid;
        auto  raw = re2c::uuid(peekRest(), uuid);

        if (!raw.isNull()) { // 0-0-0-0-0 is a null but valid UUID
            advance(raw.length());
            return QSharedPointer<UuidNode>::create(
                spanText(raw), std::move(uuid), true);
        } else {
            reportError(QT_TR_NOOP("Invalid UUID"));
            return QSharedPointer<UuidNode>::create(QString(), QUuid(), false);
        }
    }

    QSharedPointer<Vec2Node> MinecraftParser::minecraft_vec2() {
        const auto &&ret = QSharedPointer<Vec2Node>::create(0);

        parseAxes(ret.get(), AxisParseOption::CanBeLocal);
        return ret;
    }

    QSharedPointer<Vec3Node> MinecraftParser::minecraft_vec3() {
        const auto &&ret = QSharedPointer<Vec3Node>::create(0);

        parseAxes(ret.get(), AxisParseOption::CanBeLocal);
        return ret;
    }

    QSharedPointer<InternalGreedyStringNode> MinecraftParser::parseGreedyString()
    {
        const int    curPos = pos();
        const auto &&raw    = getWithRegex(QStringLiteral(R"([^\s\\'"]+)"));

        if (!raw.isEmpty()) {
            return QSharedPointer<InternalGreedyStringNode>::create(
                spanText(raw), true);
        } else {
            reportError(QT_TR_NOOP("Invalid empty greedy string"), {}, curPos,
                        raw.length());
            return QSharedPointer<InternalGreedyStringNode>::create(QString(),
                                                                    false);
        }
    }

    QSharedPointer<InternalRegexPatternNode> MinecraftParser::parseRegexPattern()
    {
        const int                curPos = pos();
        const auto             &&rest   = getRest();
        const QRegularExpression pattern(rest.toString());

        if (pattern.isValid()) {
            return QSharedPointer<InternalRegexPatternNode>::create(
                spanText(rest), std::move(pattern), true);
        } else {
            reportError(pattern.errorString().toStdString().c_str(),
                        {}, curPos + pattern.patternErrorOffset(), 1);
            return QSharedPointer<InternalRegexPatternNode>::create(
                QString(), QRegularExpression(), false);
        }
    }

    void MinecraftParser::setGameVer(const QVersionNumber &newGameVer,
                                     const bool autoLoadSchema) {
        gameVer = newGameVer;

        if (autoLoadSchema) {
            loadSchema(QStringLiteral(":/minecraft/") + newGameVer.toString() +
                       QStringLiteral("/summary/commands/data.min.json"));
        }
    }
}
