#include "minecraftparser.h"

#include "json/single_include/nlohmann/json.hpp"

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

    QString MinecraftParser::oneOf(const QStringList &strArr) {
        const int         start   = pos();
        const QStringRef &curText = this->text().midRef(this->pos());

        for (const QString &str: qAsConst(strArr)) {
            if (curText.startsWith(str)) {
                this->advance(str.length());
                return str;
            }
        }
        this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                      "Only accept one of the following: %1"),
                    { strArr.join(QLatin1String(", ")) }, start,
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
        static const QString mixCoordErrMsg(QT_TRANSLATE_NOOP(
                                                "Parser::Error",
                                                "Cannot mix world & local coordinates (everything must either use ^ or not)."));

        const bool canBeLocal = options & AxisParseOption::CanBeLocal;
        const bool onlyInt    = options & AxisParseOption::OnlyInteger;

        const auto axisVal =
            [this, onlyInt](AngleNode *axis) {
                if (onlyInt) {
                    const auto &&intNode = brigadier_integer();
                    axis->setValue(intNode->value());
                } else {
                    const auto &&floatNode = brigadier_float();
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
                this->error(mixCoordErrMsg);
            }
        } else if (this->curChar() == '^') {
            if (isLocal == false)
                isLocal = true;
            if (canBeLocal && isLocal) {
                axis      = AnglePtr::create(AxisType::Local);
                hasPrefix = true;
                this->advance();
            } else if (!isLocal) {
                this->error(mixCoordErrMsg);
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
                    this->error(mixCoordErrMsg);
                }
            } else {
                axisVal(axis.get());
            }
        }
        axis->setText(spanText(start));
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
                axes->setFirstAxis(
                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
                                             peekLiteral(), options, isLocal));
                axes->firstAxis()->setTrailingTrivia(eat(' '));
                axes->setSecondAxis(
                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
                                             peekLiteral(), options, isLocal));
                break;
            }

            case PT::BlockPos:
            case PT::Vec3: {
                auto *axes = static_cast<XyzNode *>(node);
                axes->setX(
                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
                                             peekLiteral(), options, isLocal));
                axes->x()->setTrailingTrivia(eat(' '));
                axes->setY(
                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
                                             peekLiteral(), options, isLocal));
                axes->y()->setTrailingTrivia(eat(' '));
                axes->setZ(
                    callWithCache<AngleNode>(&MinecraftParser::parseAxis, this,
                                             peekLiteral(), options, isLocal));
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
                switch (curChar().toLower().toLatin1()) {
                    case 'b': {
                        return parseArrayTag<NbtByteArrayNode, NbtByteNode>(
                            "Unvaild SNBT tag in a byte array tag");
                    }

                    case 'i': {
                        return parseArrayTag<NbtIntArrayNode, NbtIntNode>(
                            "Unvaild SNBT tag in an integer array tag");
                    }

                    case 'l': {
                        return parseArrayTag<NbtLongArrayNode, NbtLongNode>(
                            "Unvaild SNBT tag in a long array tag");
                    }

                    default: {
                        return parseListTag();
                    }
                }
            }

            case '"':
            case '\'': {
                return QSharedPointer<NbtStringNode>::create(spanText(start),
                                                             getQuotedString());
            }

            default: {
                if (curChar().isNumber() || curChar() == '-' ||
                    curChar() == '.') {
                    return parseNumericTag();
                } else if (const auto &&boolean = brigadier_bool()) {
                    return QSharedPointer<NbtByteNode>::create(boolean->text(),
                                                               boolean->value());
                } else {
                    const QString &&value =
                        getWithCharset(QStringLiteral("a-zA-Z0-9-_."));
                    if (value.isEmpty())
                        error(QT_TRANSLATE_NOOP("Parser::Error",
                                                "Invaild empty tag value"));
                    return QSharedPointer<NbtStringNode>::create(spanText(value));
                }
            }
        }
        return nullptr;
    }

    QSharedPointer<NbtNode> MinecraftParser::parseNumericTag() {
        const int    start  = pos();
        const auto &&number = brigadier_double();

        const QString &&literal = number->text();
        bool            ok      = false;

        switch (curChar().toLower().toLatin1()) {
            case 'b': {
                advance();
                float integer = literal.toFloat(&ok);
                if (ok && integer <= 128 && integer >= -127) {
                    return QSharedPointer<NbtByteNode>::create(literal,
                                                               (int8_t)integer);
                } else {
                    error(QT_TRANSLATE_NOOP("Parser::Error",
                                            "%1 is not a vaild SNBT byte tag"),
                          { literal }, start, number->length());
                    break;
                }
            }

            case 'd': {
                advance();
                return QSharedPointer<NbtDoubleNode>::create(literal,
                                                             number->value());
            }

            case 'f': {
                advance();
                float value = literal.toFloat(&ok);
                if (ok) {
                    return QSharedPointer<NbtFloatNode>::create(literal, value);
                } else {
                    error(QT_TRANSLATE_NOOP("Parser::Error",
                                            "%1 is not a vaild SNBT float tag"),
                          { literal }, start, number->length());
                    break;
                }
            }

            case 'l': {
                advance();
                int64_t value = literal.toLongLong(&ok);
                if (ok) {
                    return QSharedPointer<NbtLongNode>::create(literal, value);
                } else {
                    error(QT_TRANSLATE_NOOP("Parser::Error",
                                            "%1 is not a vaild SNBT long tag"),
                          { literal }, start, number->length());
                    break;
                }
            }

            case 's': {
                advance();
                short value = literal.toFloat(&ok);
                if (ok) {
                    return QSharedPointer<NbtShortNode>::create(literal, value);
                } else {
                    error(QT_TRANSLATE_NOOP("Parser::Error",
                                            "%1 is not a vaild SNBT short tag"),
                          { literal }, start, number->length());
                    break;
                }
            }

            default: {
                if (literal.contains('.')) {
                    return QSharedPointer<NbtDoubleNode>::create(literal,
                                                                 number->value());
                } else {
                    int value = literal.toInt(&ok);
                    if (ok) {
                        return QSharedPointer<NbtIntNode>::create(literal,
                                                                  value);
                    } else {
                        error(QT_TRANSLATE_NOOP("Parser::Error",
                                                "%1 is not a vaild SNBT integer tag"),
                              { literal },
                              start,
                              number->length());
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
                error(QT_TRANSLATE_NOOP("Parser::Error",
                                        "Type of elements in this list tag must be the same"));
            }
            elem->setTrailingTrivia(eatListSep(',',
                                               ']'));
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
                    const auto &&ret = brigadier_bool();
                    if (ret->isValid())
                        return ret;
                    else
                        this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                                      "Argument value must be boolean"));
                    return nullptr;
                }, false, R"(a-zA-z0-9-_:.+/)");
            } else {
                const auto &&ret = brigadier_bool();
                if (ret->isValid())
                    return ret;
                else
                    this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                                  "Argument value must be a boolean"));
                return nullptr;
            }
            return nullptr;
        }, false, R"(a-zA-z0-9-_:.+/)");
    }

    QSharedPointer<MapNode> MinecraftParser::parseEntityArguments() {
        return this->parseMap<MapNode, ParseNode>(
            '[', ']', '=', [this](
                const QString &key) -> QSharedPointer<ParseNode> {
            static const QStringList doubleValKeys { "x", "y", "z", "dx", "dy", "dz" };
            static const QStringList rangeValKeys { "distance", "x_rotation", "y_rotation" };
            if (doubleValKeys.contains(
                    key)) {
                return brigadier_double();
            } else if (rangeValKeys.contains(key)) {
                return minecraft_floatRange();
            } else if (key == QLatin1String("level")) {
                return minecraft_intRange();
            } else if (key == QLatin1String("limit")) {
                return brigadier_integer();
            } else if (key == QLatin1String("predicate")) {
                const auto &&ret = parseNegEntityArg();
                ret->setNode(minecraft_resourceLocation());
                return ret;
            } else if (key == QLatin1String("sort")) {
                const QString &&literal = this->oneOf({ "nearest", "furthest",
                                                        "random",
                                                        "arbitrary" });
                return QSharedPointer<StringNode>::create(spanText(literal));
            } else if (key == QLatin1String("gamemode")) {
                const auto &&ret        = parseNegEntityArg();
                const QString &&literal = this->oneOf({ "adventure", "creative",
                                                        "spectator",
                                                        "survival" });
                ret->setNode(QSharedPointer<StringNode>::create(spanText(
                                                                    literal)));
                return ret;
            } else if (key == QLatin1String("name")) {
                const int start  = pos();
                const auto &&ret = parseNegEntityArg();
                QString literal;
                if (curChar() == '"' || curChar() == '\'') {
                    literal = getQuotedString();
                } else {
                    literal = this->getWithCharset("0-9a-zA-Z-_.+");
                }
                ret->setNode(QSharedPointer<StringNode>::create(spanText(start),
                                                                literal));
                return ret;
            } else if (key == QLatin1String("type")) {
                const auto &&ret    = parseNegEntityArg();
                const auto &&resLoc = QSharedPointer<ResourceLocationNode>::create(
                    0);

                parseResourceLocation(resLoc.get(), true);
                ret->setNode(resLoc);
                return ret;
            } else if (key == QLatin1String("nbt")) {
                const auto &&ret = parseNegEntityArg();
                ret->setNode(parseCompoundTag());
                return ret;
            } else if (key == QLatin1String("tag") || key == QLatin1String(
                           "team")) {
                const auto &&ret  = parseNegEntityArg();
                QString &&literal = getWithCharset("0-9a-zA-Z-_.+");
                ret->setNode(QSharedPointer<StringNode>::create(spanText(
                                                                    literal)));
                return ret;
            } else if (key == QLatin1String("scores")) {
                return parseMap<MapNode, IntRangeNode>(
                    '{', '}', '=', [this](const QString &) {
                    return minecraft_intRange();
                }, false, R"(0-9a-zA-Z-_.+)");
            } else if (key == QLatin1String("advancements")) {
                return parseEntityAdvancements();
            } else {
                this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                              "Unknown entity argument name: %1"),
                            { key });
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
                error(QT_TRANSLATE_NOOP("Parser::Error",
                                        "Invaild target selector variable: %1"),
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
                ret->setName(QSharedPointer<StringNode>::create(spanText(start),
                                                                getQuotedString()));
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
                        ret->setFilter(minecraft_nbtCompoundTag());
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
                ret->setName(QSharedPointer<StringNode>::create(
                                 spanText(getWithCharset("a-zA-Z0-9-_+"))));
                /*qDebug() << "After key" << ret->name()->value(); */
                if (ret->name()->value().isEmpty())
                    error(QT_TRANSLATE_NOOP("Parser::Error",
                                            "Invaild empty NBT path key"));
                if (curChar() == '{')
                    ret->setFilter(minecraft_nbtCompoundTag());
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
        const static QRegularExpression charsetRegex{ "[0-9a-z-_\\/.]*" };

        const int start = pos();
        SpanPtr   nspace;
        SpanPtr   id;

        if (curChar() == '#') {
            if (acceptTag) {
                node->setIsTag(true);
                advance();
                node->setLeftText(spanText(start));
            } else {
                error(QT_TR_NOOP("A resource location tag is not allowed here."));
            }
        }

        id = SpanPtr::create(spanText(getWithRegex(charsetRegex)));
        if (this->curChar() == ':') {
            this->advance();
            nspace = std::move(id);
            id     = SpanPtr::create(spanText(getWithRegex(charsetRegex)));
            id->setLeadingTrivia(spanText(QStringLiteral(":")));
        }
        if (nspace && nspace->text().contains('/'))
            this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                          "The character '/' is not allowed in the namespace"),
                        {},
                        start,
                        nspace->length());
        Q_ASSERT(id != nullptr);
        if (id->leftText().isNull() && id->text().isEmpty()) {
            this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                          "Invaild empty namespaced ID"));
        }

        if (nspace) {
            node->setNamespace(std::move(nspace));
        }
        node->setId(std::move(id));

        node->setLength(pos() - start + 1);
    }

    void MinecraftParser::parseBlock(BlockStateNode *node, bool acceptTag) {
        parseResourceLocation(node, acceptTag);

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
            case ParserType::GameProfile: { return minecraft_gameProfile(props);
            }
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
        const static QStringList colors {
            "aqua", "black", "blue",
            "dark_aqua", "dark_blue",
            "dark_green", "dark_grey",
            "dark_purple", "dark_red",
            "gold", "green", "grey",
            "light_purple", "red",
            "reset", "white", "yellow",
        };
        const QString &&literal = oneOf(colors);

        return QSharedPointer<ColorNode>::create(spanText(literal));
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
        const int curPos = pos();

        try {
            const auto &&rest = getRest();
            const json &&j    = json::parse(rest.toString().toStdString());
            const auto &&ret  =
                QSharedPointer<ComponentNode>::create(spanText(rest));
            ret->setValue(j);
            return ret;
        }  catch (const json::parse_error &err) {
            /* TODO: Process JSON errors for localization */
            error(err.what(), {}, curPos + err.byte - 1);
        }
        return nullptr;
    }

    QSharedPointer<DimensionNode> MinecraftParser::
    minecraft_dimension() {
        const auto &&ret = QSharedPointer<DimensionNode>::create(0);

        parseResourceLocation(ret.get());
        return ret;
    }

    QSharedPointer<EntityNode> MinecraftParser::minecraft_entity(
        const QVariantMap &props) {
        const auto &&ret    = QSharedPointer<EntityNode>::create(0);
        int          curPos = pos();

        if (this->curChar() == '@') {
            ret->setNode(parseTargetSelector());
            ;
        } else {
            const auto &&uuid = minecraft_uuid();
            if (!uuid->value().isNull()) {
                ret->setNode(std::move(uuid));
            } else {
                setPos(curPos);
                const QString &&literal = getWithCharset("0-9a-zA-Z-_#$%.§");
                if (literal.isEmpty()) {
                    this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                                  "Invaild empty player name"));
                }
                const auto &&playerName = QSharedPointer<StringNode>::create(spanText(
                                                                                 literal));
                ret->setNode(std::move(playerName));
            }
        }
        if (!ret) {
            error(QT_TRANSLATE_NOOP("Parser::Error",
                                    "Cannot parse entity"));
            return nullptr;
        }
        ret->setPlayerOnly(props["type"] == "players");
        ret->setSingleOnly(props["amount"] == "single");
        return ret;
    }

    QSharedPointer<EntityAnchorNode> MinecraftParser::
    minecraft_entityAnchor() {
        const QString &&literal = oneOf({ "eyes", "feet", });

        return QSharedPointer<EntityAnchorNode>::create(spanText(literal));
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
            if (this->peek(2) == QLatin1String("..")) {
                this->advance(2);
                hasDoubleDot = true;
            } else if (text().midRef(pos() - 1, 2) == QLatin1String("..")) {
                this->advance();
                hasDoubleDot = true;
            }
            if (hasDoubleDot) {
                if (curChar().isDigit() || curChar() == '.' ||
                    curChar() == '-') { // "min..max"
                    ret->setMaxValue(this->brigadier_float(), true);
                }
                ret->setMinValue(std::move(num1), true); // "min.."
                num1->setTrailingTrivia(spanText(QStringLiteral("..")));
            } else {
                ret->setExactValue(std::move(num1)); // "value"
            }
        } else {                                     // "..max"
            ret->setMaxValue(std::move(num1), false);
            num1->setLeadingTrivia(spanText(QStringLiteral("..")));
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

    QSharedPointer<GameProfileNode> MinecraftParser::
    minecraft_gameProfile(const QVariantMap &props) {
        const auto &&entity = minecraft_entity(props);

        return QSharedPointer<GameProfileNode>::create(entity.get());
    }

    QSharedPointer<IntRangeNode> MinecraftParser::
    minecraft_intRange(const QVariantMap &props) {
        const int    start  = pos();
        const auto &&ret    = QSharedPointer<IntRangeNode>::create(0);
        bool         hasMax = false;

        if (this->peek(2) == QLatin1String("..")) {
            hasMax = true;
            this->advance(2);
        }
        const auto &&num1 = this->brigadier_integer();
        if (!hasMax) {
            if (this->peek(2) == QLatin1String("..")) {
                this->advance(2);
                if (curChar().isDigit() || curChar() == '-') { // "min..max"
                    ret->setMaxValue(brigadier_integer(), true);
                }
                ret->setMinValue(std::move(num1), true); // "min.."
                num1->setTrailingTrivia(spanText(QStringLiteral("..")));
            } else {
                ret->setExactValue(std::move(num1)); // "value"
            }
        } else {                                     // "..max"
            ret->setMaxValue(std::move(num1), false);
            num1->setLeadingTrivia(spanText(QStringLiteral("..")));
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
        const QString &&objname = this->getWithCharset("a-z0-9._");

        return QSharedPointer<ItemSlotNode>::create(objname);
    }

    QSharedPointer<ItemStackNode> MinecraftParser::minecraft_itemStack() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<ItemStackNode>::create(0);

        parseResourceLocation(ret.get());

        if (this->curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<ItemPredicateNode> MinecraftParser::
    minecraft_itemPredicate() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<ItemPredicateNode>::create(0);

        parseResourceLocation(ret.get(), true);

        if (this->curChar() == '{') {
            ret->setNbt(parseCompoundTag());
        }
        ret->setLength(pos() - start);
        return ret;
    }

    QSharedPointer<MessageNode> MinecraftParser::minecraft_message() {
        return QSharedPointer<MessageNode>::create(spanText(getRest()));
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
                error(QT_TRANSLATE_NOOP("Parser::Error",
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
        QString &&objname = this->getWithCharset("0-9a-zA-Z-+_.#");

        if ((objname.length() > 16) && (gameVer < QVersionNumber(1, 18, 2))) {
            this->error(QT_TRANSLATE_NOOP("Parser::Error",
                                          "Objective '%1' must be less than 16 characters"),
                        { objname },
                        curPos,
                        objname.length());
        }
        return QSharedPointer<ObjectiveNode>::create(spanText(objname));
    }

    QSharedPointer<ObjectiveCriteriaNode> MinecraftParser::
    minecraft_objectiveCriteria() {
        const QString &&criteria = this->getWithCharset("-\\w.:");

        return QSharedPointer<ObjectiveCriteriaNode>::create(spanText(criteria));
    }

    QSharedPointer<OperationNode> MinecraftParser::
    minecraft_operation() {
        static const QStringList operators {
            "=", "<", ">", "><", "+=", "-=", "*=", "/=", "%=",
        };
        const QString &&literal = oneOf(operators);

        return QSharedPointer<OperationNode>::create(spanText(literal));
    }

    QSharedPointer<ParticleNode> MinecraftParser::
    minecraft_particle() {
        const int    start = pos();
        const auto &&ret   = QSharedPointer<ParticleNode>::create(0);

        parseResourceLocation(ret.get());

        QString fullId;
        if (!ret->nspace()) {
            fullId = ret->id()->text();
        } else {
            const QString &&nspace = ret->nspace()->text();
            if (nspace.isEmpty() || (nspace == "minecraft")) {
                fullId = ret->id()->text();
            } else {
                fullId = nspace + ":" + ret->id()->text();
            }
        }

        if (fullId == QLatin1String("block") ||
            fullId == QLatin1String("block_marker") ||
            fullId == QLatin1String("falling_dust")) {
            ret->setLeadingTrivia(eat(' '));
            ret->setParams({ minecraft_blockState() });
        } else if (fullId == QLatin1String("dust")) {
            ret->setLeadingTrivia(eat(' '));
            const auto &color = parseParticleColor();
            color->setTrailingTrivia(eat(' '));
            ret->setParams({ color, brigadier_float() });
        } else if (fullId == "dust_color_transition") {
            ret->setLeadingTrivia(eat(' '));
            const auto &startColor = parseParticleColor();
            startColor->setTrailingTrivia(eat(' '));
            const auto &size = brigadier_float();
            size->setTrailingTrivia(eat(' '));
            const auto &endColor = parseParticleColor();
            ret->setParams({ startColor, size, endColor });
        } else if (fullId == QLatin1String("item")) {
            ret->setLeadingTrivia(eat(' '));
            ret->setParams({ minecraft_itemStack() });
        }
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
        auto &&ret = QSharedPointer<ScoreHolderNode>::create(1);

        if (curChar() == '*') {
            ret->setAll(true);
            advance();
        } else {
            const auto &&entity = minecraft_entity(props);
            ret = QSharedPointer<ScoreHolderNode>::create(entity.get());
        }
        return ret;
    }

    QSharedPointer<ScoreboardSlotNode> MinecraftParser::
    minecraft_scoreboardSlot() {
        const static QStringList scoreboardSlots = {
            "sidebar",
            "belowName",
            "sidebar.team.aqua",
            "sidebar.team.black",
            "sidebar.team.blue",
            "sidebar.team.dark_aqua",
            "sidebar.team.dark_blue",
            "sidebar.team.dark_green",
            "sidebar.team.dark_grey",
            "sidebar.team.dark_purple",
            "sidebar.team.dark_red",
            "sidebar.team.gold",
            "sidebar.team.green",
            "sidebar.team.grey",
            "sidebar.team.light_purple",
            "sidebar.team.red",
            "sidebar.team.reset",
            "sidebar.team.white",
            "sidebar.team.yellow",
        };

        const QString &&slot = this->oneOf(scoreboardSlots);

        return QSharedPointer<ScoreboardSlotNode>::create(spanText(slot));
    }

    QSharedPointer<SwizzleNode> MinecraftParser::
    minecraft_swizzle() {
        const int         start = pos();
        SwizzleNode::Axes axes;
        QString           acceptedChars("xyz");

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
            this->getWithCharset(QStringLiteral("-+\\w.:"));

        return QSharedPointer<TeamNode>::create(spanText(literal));
    }

    QSharedPointer<TimeNode> MinecraftParser::minecraft_time() {
        const int    curPos = pos();
        auto         unit   = TimeNode::Unit::Tick;
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
        return QSharedPointer<TimeNode>::create(spanText(curPos),
                                                number->value(), unit);
    }

    QSharedPointer<UuidNode> MinecraftParser::minecraft_uuid() {
        const int                       curPos = pos();
        static const QRegularExpression regex(
            R"(([\da-fA-F]{1,8})-([\da-fA-F]{1,4})-([\da-fA-F]{1,4})-([\da-fA-F]{1,4})-([\da-fA-F]{1,12}))");
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
            return QSharedPointer<UuidNode>::create(spanText(
                                                        match.capturedRef()),
                                                    hexList.join('-'));
        } else {
            return QSharedPointer<UuidNode>::create(QString(), QUuid());
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
