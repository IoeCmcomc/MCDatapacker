#include "minecraftparser.h"

#include "json/single_include/nlohmann/json.hpp"
#include "uberswitch/include/uberswitch/uberswitch.hpp"

#include <QMetaEnum>

using json = nlohmann::json;

namespace Command {
    MinecraftParser::MinecraftParser() {
        /*
           connect(this, &QObject::destroyed, [ = ](QObject *obj) {
           qDebug() << "MinecraftParser emit QObject destroyed:" << obj;
           });
         */
    }

    QString join(const MinecraftParser::QLatin1StringVector &vector) {
        QString ret;

        for (auto it = vector.cbegin(); it != vector.cend();) {
            ret += *it;
            ++it;
            if (it != vector.cend()) {
                ret += ", "_QL1;
            }
        }
        return ret;
    }

    QString MinecraftParser::oneOf(const QLatin1StringVector &strArr) {
        const int         start   = pos();
        const QStringRef &curText = text().midRef(pos());

        for (const auto &str: qAsConst(strArr)) {
            if (curText.startsWith(str)) {
                this->advance(str.size());
                return str;
            }
        }
        this->reportError(QT_TR_NOOP("Only accept one of the following: %1"),
                          { join(strArr) }, start,
                          peekLiteral().length());
        return QString();
    }

    QString MinecraftParser::eatListSep(QChar sepChr, QChar endChr) {
        const int start = pos();

        while (curChar().isSpace()) {
            advance();
        }
        if (this->curChar() != endChr) {
            expect(sepChr);
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
        bool       valid      = true;

        const auto axisVal =
            [this, onlyInt, &valid](AngleNode *axis) {
                if (onlyInt) {
                    const auto &&intNode = brigadier_integer();
                    valid = intNode->isValid();
                    axis->setValue(intNode->value());
                } else {
                    const auto &&floatNode = brigadier_float();
                    valid = floatNode->isValid();
                    axis->setValue(floatNode->value());
                }
            };

        const int start     = pos();
        bool      hasPrefix = false;
        AnglePtr  axis;
        if (this->curChar() == '~') {
            if (!isLocal) {
                axis      = AnglePtr::create(AxisType::Relative);
                hasPrefix = true;
                isLocal   = false;
                this->advance();
            } else {
                this->throwError(mixCoordErrMsg);
            }
        } else if (this->curChar() == '^') {
            if (isLocal == false)
                isLocal = true;
            if (canBeLocal && isLocal) {
                axis      = AnglePtr::create(AxisType::Local);
                hasPrefix = true;
                this->advance();
            } else if (!isLocal) {
                this->throwError(mixCoordErrMsg);
            }
        } else {
            axis = AnglePtr::create(AxisType::Absolute);
        }
        if (hasPrefix) {
            if ((!this->curChar().isNull()) && (this->curChar() != ' ')) {
                axisVal(axis.get());
            }
        } else {
            if (isLocal) {
                if (this->curChar() != ' ') {
                    this->throwError(mixCoordErrMsg);
                }
            } else {
                axisVal(axis.get());
            }
        }
        axis->setText(spanText(start));
        axis->setIsValid(valid);
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
                axes->setFirstAxis(parseAxis(options, isLocal));
                axes->firstAxis()->setTrailingTrivia(eat(' '));
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
                axes->setX(parseAxis(options, isLocal));
                axes->x()->setTrailingTrivia(eat(' '));
//                axes->setY(
//                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
//                                             peekLiteral(), options, isLocal));
                axes->setY(parseAxis(options, isLocal));
                axes->y()->setTrailingTrivia(eat(' '));
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
                if (text().midRef(pos() + 1, 1) == ';') {
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
                const QString &&value = getLiteralString();
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
        const int    start  = pos();
        const auto &&number = brigadier_double();

        const QString &&literal = number->text();
        bool            ok      = false;

        switch (curChar().toLower().toLatin1()) {
            case 'b': {
                advance();
                short int integer = literal.toShort(&ok);
                if (ok && integer <= 128 && integer >= -127) {
                    return QSharedPointer<NbtByteNode>::create(
                        spanText(start), (int8_t)integer, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT byte tag"),
                               { literal }, start, number->length());
                    break;
                }
            }

            case 'd': {
                advance();
                return QSharedPointer<NbtDoubleNode>::create(
                    spanText(start), number->value(), true);
            }

            case 'f': {
                advance();
                float value = literal.toFloat(&ok);
                if (ok) {
                    return QSharedPointer<NbtFloatNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT float tag"),
                               { literal }, start, number->length());
                    break;
                }
            }

            case 'l': {
                advance();
                int64_t value = literal.toLongLong(&ok);
                if (ok) {
                    return QSharedPointer<NbtLongNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT long tag"),
                               { literal }, start, number->length());
                    break;
                }
            }

            case 's': {
                advance();
                short int value = literal.toShort(&ok);
                if (ok) {
                    return QSharedPointer<NbtShortNode>::create(
                        spanText(start), value, true);
                } else {
                    throwError(QT_TR_NOOP("%1 is not a vaild SNBT short tag"),
                               { literal }, start, number->length());
                    break;
                }
            }

            default: {
                if (literal.contains('.')) {
                    return QSharedPointer<NbtDoubleNode>::create(
                        literal, number->value(), true);
                } else {
                    int value = literal.toInt(&ok);
                    if (ok) {
                        return QSharedPointer<NbtIntNode>::create(
                            literal, value, true);
                    } else {
                        throwError(
                            QT_TR_NOOP(
                                "%1 is not a vaild SNBT integer tag"),
                            { literal }, start, number->length());
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
                reportError(QT_TR_NOOP(
                                "Type of elements in this list tag must be the same"));
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
            if (this->curChar() == '{') {
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
        return this->parseMap<MapNode, ParseNode>(
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
                    const auto &&ret = parseNegEntityArg();
                    ret->setNode(minecraft_resourceLocation());
                    return ret;
                }
                ucase ("sort"_QL1): {
                    const QString &&literal =
                        this->oneOf({ "nearest"_QL1, "furthest"_QL1,
                                      "random"_QL1, "arbitrary"_QL1 });
                    return QSharedPointer<StringNode>::create(
                        spanText(literal), !literal.isEmpty());
                }
                ucase ("gamemode"_QL1): {
                    const auto &&ret        = parseNegEntityArg();
                    const QString &&literal =
                        this->oneOf({ "adventure"_QL1, "creative"_QL1,
                                      "spectator"_QL1, "survival"_QL1 });
                    ret->setNode(QSharedPointer<StringNode>::create(
                                     spanText(literal), !literal.isEmpty()));
                    return ret;
                }
                ucase ("name"_QL1): {
                    const int start  = pos();
                    const auto &&ret = parseNegEntityArg();
                    QString literal;
                    if (curChar() == '"' || curChar() == '\'') {
                        literal = getQuotedString();
                    } else {
                        literal = getLiteralString();
                    }
                    ret->setNode(QSharedPointer<StringNode>::create(
                                     spanText(start), literal,
                                     errorIfNot(
                                         !literal.isEmpty(),
                                         "Invalid empty name in target selector")));
                    return ret;
                }
                ucase ("type"_QL1): {
                    const auto &&ret    = parseNegEntityArg();
                    const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(
                        0);

                    parseResourceLocation(resLoc.get(), true);
                    ret->setNode(resLoc);
                    return ret;
                }
                ucase ("nbt"_QL1): {
                    const auto &&ret = parseNegEntityArg();
                    ret->setNode(parseCompoundTag());
                    return ret;
                }
                ucase ("tag"_QL1):
                ucase ("team"_QL1): {
                    const auto &&ret  = parseNegEntityArg();
                    QString &&literal = getLiteralString();
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
                default: {
                    this->throwError(QT_TR_NOOP(
                                         "Unknown entity argument name: %1"),
                                     { key });
                }
            }
            return nullptr;
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

            default: {
                const auto &&name = getWithCharset("0-9a-zA-Z-_+"_QL1);
                ret->setName(QSharedPointer<StringNode>::create(
                                 spanText(name),
                                 errorIfNot(!name.isEmpty(),
                                            QT_TR_NOOP(
                                                "Invalid empty NBT path key"))));
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
        color->r()->setTrailingTrivia(eat(' '));
        color->setG(brigadier_float());
        color->g()->setTrailingTrivia(eat(' '));
        color->setB(brigadier_float());
        color->setLength(pos() - start);
        return color;
    }

    QSharedPointer<EntityArgumentValueNode> MinecraftParser::parseNegEntityArg()
    {
        const bool   isNegative = curChar() == '!';
        const auto &&ret        =
            QSharedPointer<EntityArgumentValueNode>::create(isNegative);

        if (isNegative) {
            advance();
            ret->setLeftText(spanText(pos() - 1));
        }
        return ret;
    }

    void MinecraftParser::parseResourceLocation(ResourceLocationNode *node,
                                                bool acceptTag) {
        const static QRegularExpression charsetRegex{ QStringLiteral(
                                                          "[0-9a-z-_\\/.]*") };

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

        id = SpanPtr::create(spanText(getWithRegex(charsetRegex)));
        if (this->curChar() == ':') {
            this->advance();
            nspace = std::move(id);
            id     = SpanPtr::create(spanText(getWithRegex(charsetRegex)));
            id->setLeadingTrivia(spanText(QStringLiteral(":")));
        }
        if (nspace) {
            if (nspace->text().contains('/')) {
                reportError(
                    QT_TR_NOOP(
                        "The character '/' is not allowed in the namespace"),
                    {}, start, nspace->length());
            } else {
                nspace->setIsValid(true);
            }
        }
        Q_ASSERT(id != nullptr);
        if (id->leftText().isNull() && id->text().isEmpty()) {
            this->reportError(QT_TR_NOOP("Invalid empty namespaced ID"));
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

        if (this->curChar() == '[') {
            node->setStates(
                parseMap<MapNode, ParseNode>('[', ']', '=',
                                             [this](const QString &) {
                const static QVariantMap props{ { "type", "word" } };
                return brigadier_string(props);
            }));
        }
        if (this->curChar() == '{')
            node->setNbt(parseCompoundTag());
    }

    void Command::MinecraftParser::parseEntity(Command::EntityNode *node,
                                               bool allowFakePlayer) {
        int curPos = pos();

        if (this->curChar() == '@') {
            node->setNode(parseTargetSelector());
        } else {
            auto &&uuid = minecraft_uuid();
            if (uuid->isValid()) {
                node->setNode(std::move(uuid));
            } else {
                setPos(curPos);
                m_errors.pop_back();
                const QString &&literal = allowFakePlayer
                        ? getUntil(QChar::Space)
                        : getWithCharset("0-9a-zA-Z_"_QL1);

                node->setNode(QSharedPointer<StringNode>::create(
                                  spanText(literal),
                                  errorIfNot(!literal.isEmpty(),
                                             QT_TR_NOOP(
                                                 "Invalid empty player name"))));
            }
        }
    }

    NodePtr MinecraftParser::invokeMethod(ArgumentNode::ParserType parserType,
                                          const QVariantMap &props) {
        using ParserType = ArgumentNode::ParserType;
        if (const auto &&ret = SchemaParser::invokeMethod(parserType, props)) {
            return ret;
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
            case ParserType::GameProfile: { return minecraft_gameProfile(); }
            case ParserType::IntRange: { return minecraft_intRange(props); }
            case ParserType::ItemEnchantment: { return minecraft_itemEnchantment();
            }
            case ParserType::ItemPredicate: { return minecraft_itemPredicate();
            }
            case ParserType::ItemSlot: { return minecraft_itemSlot(); }
            case ParserType::ItemStack: { return minecraft_itemStack(); }
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
            case ParserType::Resource: { return minecraft_resource(props); }
            case ParserType::ResourceOrTag: {
                return minecraft_resourceOrTag(props);
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
            case ParserType::Swizzle: { return minecraft_swizzle(); }
            case ParserType::Team: { return minecraft_team(); }
            case ParserType::Time: { return minecraft_time(); }
            case ParserType::Uuid: { return minecraft_uuid(); }
            case ParserType::Vec2: { return minecraft_vec2(); }
            case ParserType::Vec3: { return minecraft_vec3(); }

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
        const static QLatin1StringVector colors {
            "aqua"_QL1, "black"_QL1, "blue"_QL1,
            "dark_aqua"_QL1, "dark_blue"_QL1,
            "dark_green"_QL1, "dark_grey"_QL1,
            "dark_purple"_QL1, "dark_red"_QL1,
            "gold"_QL1, "green"_QL1, "grey"_QL1,
            "light_purple"_QL1, "red"_QL1,
            "reset"_QL1, "white"_QL1, "yellow"_QL1,
        };
        const QString &&literal = oneOf(colors);

        return QSharedPointer<ColorNode>::create(spanText(literal),
                                                 !literal.isEmpty());
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
        const QString &&literal = oneOf({ "eyes"_QL1, "feet"_QL1, });

        return QSharedPointer<EntityAnchorNode>::create(spanText(literal),
                                                        !literal.isEmpty());
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

        if (this->peek(2) == QLatin1String("..")) {
            hasMax = true;
            this->advance(2);
        }

        const auto &&num1 = this->brigadier_float();
        if (!hasMax) {
            bool hasDoubleDot = false;
            if (this->peek(2) == ".."_QL1) {
                this->advance(2);
                hasDoubleDot = true;
            } else if (!num1->text().isEmpty() && num1->text().back() == '.'_QL1
                       && curChar() == '.'_QL1) {
                this->advance();
                num1->chopTrailingDot();
                hasDoubleDot = true;
            }
            if (hasDoubleDot) {
                if (curChar().isDigit() || curChar() == '.' ||
                    curChar() == '-') { // "min..max"
                    ret->setMaxValue(this->brigadier_float(), true);
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

    QSharedPointer<GameProfileNode> MinecraftParser::minecraft_gameProfile() {
        const auto &&ret = QSharedPointer<GameProfileNode>::create(0);

        ret->setPlayerOnly(true);

        parseEntity(ret.get(), false);
        return ret;
    }

    QSharedPointer<IntRangeNode> MinecraftParser::
    minecraft_intRange(const QVariantMap &props) {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<IntRangeNode>::create(0);
        bool         hasMax = false;

        if (this->peek(2) == ".."_QL1) {
            hasMax = true;
            this->advance(2);
        }
        const auto &&num1 = this->brigadier_integer();
        if (!hasMax) {
            if (this->peek(2) == ".."_QL1) {
                this->advance(2);
                if (curChar().isDigit() || curChar() == '-') { // "min..max"
                    ret->setMaxValue(brigadier_integer(), false);
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

    QSharedPointer<ItemSlotNode> MinecraftParser::
    minecraft_itemSlot() {
        const QString &&slot = this->getWithCharset("a-z0-9._"_QL1);

        return QSharedPointer<ItemSlotNode>::create(
            spanText(slot), errorIfNot(!slot.isEmpty(),
                                       "Invalid empty item slot"));
    }

    QSharedPointer<ItemStackNode> MinecraftParser::minecraft_itemStack() {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<ItemStackNode>::create(0);
        const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(0);

        parseResourceLocation(resLoc.get());
        ret->setResLoc(std::move(resLoc));

        if (this->curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ItemPredicateNode> MinecraftParser::
    minecraft_itemPredicate() {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<ItemPredicateNode>::create(0);
        const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(0);

        parseResourceLocation(resLoc.get(), true);
        ret->setResLoc(std::move(resLoc));

        if (this->curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
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
               curChar() == '"') {
            const auto &&step = parseNbtPathStep();

            if ((step->type() == NbtPathStepNode::Type::Key)
                && (!(last->trailingTrivia() == '.'))) {
                reportError(QT_TR_NOOP(
                                "Missing character '.' before a named tag"),
                            {},
                            start + last->length() - 1);
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
        const int curPos  = pos();
        QString &&objname = this->getWithCharset("0-9a-zA-Z-+_.#"_QL1);
        bool      valid   = true;

        if (objname.isEmpty()) {
            reportError("Invalid empty objective");
            valid = false;
        } else if ((objname.length() > 16) &&
                   (gameVer < QVersionNumber(1, 18, 2))) {
            reportError(QT_TR_NOOP(
                            "Objective '%1' must be less than 16 characters"),
                        { objname }, curPos, objname.length());
            valid = false;
        }
        return QSharedPointer<ObjectiveNode>::create(spanText(objname), valid);
    }

    QSharedPointer<ObjectiveCriteriaNode> MinecraftParser::
    minecraft_objectiveCriteria() {
        const QString &&criteria = this->getWithCharset("-\\w.:"_QL1);

        return QSharedPointer<ObjectiveCriteriaNode>::create(
            spanText(criteria), errorIfNot(!criteria.isEmpty(),
                                           "Invalid empty objective criteria"));
    }

    QSharedPointer<OperationNode> MinecraftParser::
    minecraft_operation() {
        static const QLatin1StringVector operators {
            "="_QL1, "<"_QL1, ">"_QL1, "><"_QL1, "+="_QL1, "-="_QL1, "*="_QL1,
            "/="_QL1, "%="_QL1,
        };
        const QString &&literal = oneOf(operators);

        return QSharedPointer<OperationNode>::create(spanText(literal),
                                                     !literal.isEmpty());
    }

    QSharedPointer<ParticleNode> MinecraftParser::minecraft_particle() {
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

        uswitch (fullId) {
            ucase ("block"_QL1):
            ucase ("block_marker"_QL1):
            ucase ("falling_dust"_QL1): {
                resLoc->setTrailingTrivia(eat(' '));
                ret->setParams(minecraft_blockState());
                break;
            }
            ucase ("dust"_QL1): {
                resLoc->setTrailingTrivia(eat(' '));
                const auto &&color = parseParticleColor();
                color->setTrailingTrivia(eat(' '));
                ret->setParams(std::move(color), brigadier_float());
                break;
            }
            ucase ("dust_color_transition"_QL1): {
                resLoc->setTrailingTrivia(eat(' '));
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
                resLoc->setTrailingTrivia(eat(' '));
                ret->setParams(minecraft_itemStack());
                break;
            }
        }
        ret->setResLoc(std::move(resLoc));
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ResourceNode> MinecraftParser::
    minecraft_resource(const QVariantMap &props) {
        const auto &&ret = QSharedPointer<ResourceNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<ResourceOrTagNode> MinecraftParser::
    minecraft_resourceOrTag(const QVariantMap &props) {
        const auto &&ret = QSharedPointer<ResourceOrTagNode>::create(0);

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
        const static QLatin1StringVector scoreboardSlots = {
            "belowName"_QL1,
            "sidebar.team.aqua"_QL1,
            "sidebar.team.black"_QL1,
            "sidebar.team.blue"_QL1,
            "sidebar.team.dark_aqua"_QL1,
            "sidebar.team.dark_blue"_QL1,
            "sidebar.team.dark_green"_QL1,
            "sidebar.team.dark_grey"_QL1,
            "sidebar.team.dark_purple"_QL1,
            "sidebar.team.dark_red"_QL1,
            "sidebar.team.gold"_QL1,
            "sidebar.team.green"_QL1,
            "sidebar.team.grey"_QL1,
            "sidebar.team.light_purple"_QL1,
            "sidebar.team.red"_QL1,
            "sidebar.team.reset"_QL1,
            "sidebar.team.white"_QL1,
            "sidebar.team.yellow"_QL1,
            "sidebar"_QL1,
        };

        const QString &&slot = this->oneOf(scoreboardSlots);

        return QSharedPointer<ScoreboardSlotNode>::create(spanText(slot),
                                                          !slot.isEmpty());
    }

    QSharedPointer<SwizzleNode> MinecraftParser::
    minecraft_swizzle() {
        const int         start = pos();
        SwizzleNode::Axes axes;
        QString           acceptedChars = QStringLiteral("xyz");

        while (acceptedChars.contains(this->curChar())) {
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
            acceptedChars = acceptedChars.replace(this->curChar(), QString());
            this->advance();
        }
        return QSharedPointer<SwizzleNode>::create(spanText(start), axes);
    }

    QSharedPointer<TeamNode> MinecraftParser::minecraft_team() {
        const QString &&literal =
            this->getWithCharset("-+\\w.:"_QL1);

        return QSharedPointer<TeamNode>::create(spanText(literal),
                                                errorIfNot(!literal.isEmpty(),
                                                           "Invalid empty team"));
    }

    QSharedPointer<TimeNode> MinecraftParser::minecraft_time() {
        const int    curPos = pos();
        auto         unit   = TimeNode::Unit::ImplicitTick;
        const auto &&number = brigadier_float();

        switch (this->curChar().toLatin1()) {
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
                                                      number->value(), unit);
        ret->setIsValid(number->isValid());
        return ret;
    }

    QSharedPointer<UuidNode> MinecraftParser::minecraft_uuid() {
        const int                       curPos = pos();
        static const QRegularExpression regex(
            QStringLiteral(
                R"(([\da-fA-F]{1,8})-([\da-fA-F]{1,4})-([\da-fA-F]{1,4})-([\da-fA-F]{1,4})-([\da-fA-F]{1,12}))"));
        const auto &&match =
            regex.match(text(), curPos, QRegularExpression::NormalMatch,
                        QRegularExpression::AnchoredMatchOption);

        if (match.hasMatch()) {
            advance(match.capturedLength());
            QStringList hexList;
            hexList << match.captured(1).rightJustified(8, '0');
            hexList << match.captured(2).rightJustified(4, '0');
            hexList << match.captured(3).rightJustified(4, '0');
            hexList << match.captured(4).rightJustified(4, '0');
            hexList << match.captured(5).rightJustified(12, '0');
            const QUuid uuid(hexList.join('-'));
            if (uuid.isNull()) {
                reportError("Invalid null UUID");
                return QSharedPointer<UuidNode>::create(QString(),
                                                        std::move(uuid), false);
            }
            return QSharedPointer<UuidNode>::create(
                spanText(match.capturedRef()), std::move(uuid), true);
        } else {
            reportError("Invalid UUID");
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

    void MinecraftParser::setGameVer(const QVersionNumber &newGameVer) {
        gameVer = newGameVer;
        setSchema(QStringLiteral(":/minecraft/") + newGameVer.toString() +
                  QStringLiteral("/summary/commands/data.min.json"));
    }
}
