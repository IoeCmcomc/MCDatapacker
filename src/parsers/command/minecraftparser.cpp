#include "minecraftparser.h"

#include "json/single_include/nlohmann/json.hpp"

#include <QMetaEnum>
using json = nlohmann::json;

Command::MinecraftParser::MinecraftParser(QObject *parent,
                                          const QString &input)
    : Command::Parser(parent, input) {
    /*printMethods(); */
    connect(this, &QObject::destroyed, [ = ](QObject *obj) {
        qDebug() << "MinecraftParser emit QObject destroyed:" << obj;
    });
}

QSharedPointer<Command::ParseNode> Command::MinecraftParser::
QVariantToParseNodeSharedPointer(const QVariant &vari) {
    if (auto ret = Parser::QVariantToParseNodeSharedPointer(vari))
        return ret;

    unsigned short int typeId = vari.userType();
    QVARIANT_CAST_SHARED_POINTER_IF_BRANCH(AxisNode)
    QVARIANT_CAST_SHARED_POINTER_IF_BRANCH(AngleNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(AxesNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(BlockStateNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ComponentNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(EntityNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(GameProfileNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ScoreHolderNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(FloatRangeNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(IntRangeNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ItemStackNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(MapNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(MultiMapNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(EntityArgumentValueNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtByteArrayNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtByteNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtCompoundNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtDoubleNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtFloatNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtIntArrayNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtIntNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtListNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtLongArrayNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtLongNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtShortNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtStringNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtPathNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(NbtPathStepNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ResourceLocationNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(BlockPosNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(BlockPredicateNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ColumnPosNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(RotationNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(Vec2Node)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(Vec3Node)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(DimensionNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(EntitySummonNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(FunctionNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ItemEnchantmentNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(MobEffectNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ColorNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(EntityAnchorNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ItemSlotNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(MessageNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ObjectiveNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ObjectiveCriteriaNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(OperationNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ScoreboardSlotNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(TeamNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(SwizzleNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(TargetSelectorNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(TimeNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(UuidNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ParticleNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ParticleColorNode)
    QVARIANT_CAST_SHARED_POINTER_ELSE_IF_BRANCH(ItemPredicateNode)
    qWarning() << "cannot cast" << vari << "to QSharedPointer<ParseNode>>";
    return nullptr;
}

QString Command::MinecraftParser::oneOf(QStringList strArr) {
    QString          str;
    const QStringRef curText = this->text().midRef(this->pos());

    for (int i = 0; i < strArr.length(); i++) {
        str = strArr[i];
        if (curText.startsWith(str)) {
            this->advance(str.length());
            return str;
        }
    }
    this->error("Argument must be one of the following: " + strArr.join(','));
    return QString();
}

QSharedPointer<Command::AxisNode> Command::MinecraftParser::parseAxis(
    AxisParseOptions options, bool &isLocal) {
    using AxisType = Command::AxisNode::AxisType;
    auto axis =
        QSharedPointer<Command::AxisNode>::create(pos());
    const bool canBeLocal     = options & AxisParseOption::CanBeLocal;
    const bool onlyInt        = options & AxisParseOption::OnlyInteger;
    QString    mixCoordErrMsg =
        "Cannot mix world & local coordinates (everything must either use ^ or not).";
    const auto axisVal = [ = ](const QSharedPointer<AxisNode> &axis) {
                             if (onlyInt) {
                                 auto intNode = brigadier_integer();
                                 axis->setValue(intNode->value());
                             } else {
                                 auto floatNode = brigadier_float();
                                 axis->setValue(floatNode->value());
                             }
                         };

    axis->setPos(pos());
    bool hasPrefix = false;
    if (this->curChar() == '~') {
        if (!isLocal) {
            axis->setType(AxisType::Relative);
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
            axis->setType(AxisType::Local);
            hasPrefix = true;
            this->advance();
        } else if (!isLocal) {
            this->error(mixCoordErrMsg);
        }
    }
    if (hasPrefix) {
        if ((!this->curChar().isNull()) && (this->curChar() != ' ')) {
            axisVal(axis);
        }
    } else {
        if (isLocal) {
            if (this->curChar() != ' ') {
                this->error(mixCoordErrMsg);
            }
        } else {
            axisVal(axis);
        }
    }
    axis->setLength(pos() - axis->pos());
    return axis;
}

QSharedPointer<Command::AxesNode> Command::MinecraftParser::parseAxes(
    AxisParseOptions options) {
    bool isLocal = false;
    auto axes    = QSharedPointer<Command::AxesNode>::create(pos());

    /*axes->setX(parseAxis(options, isLocal)); */
    axes->setX(callWithCache<AxisNode>(&MinecraftParser::parseAxis, this,
                                       peekLiteral(), options, isLocal));
    if (options & AxisParseOption::ParseY) {
        this->eat(' ');
        axes->setY(callWithCache<AxisNode>(&MinecraftParser::parseAxis, this,
                                           peekLiteral(), options, isLocal));
    }

    this->eat(' ');
    axes->setZ(callWithCache<AxisNode>(&MinecraftParser::parseAxis, this,
                                       peekLiteral(), options, isLocal));
    axes->setLength(pos() - axes->pos());
    return axes;
}

QSharedPointer<Command::NbtCompoundNode> Command::MinecraftParser::
parseCompoundTag() {
    return parseMap<NbtCompoundNode, NbtNode>('{', '}', ':',
                                              [this](const QString &key)
                                              -> QSharedPointer<NbtNode> {
        return parseTagValue();
    }, true);
}

QSharedPointer<Command::NbtNode> Command::MinecraftParser::parseTagValue() {
    const int startPos = pos();

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
        return QSharedPointer<Command::NbtStringNode>::create(startPos,
                                                              getQuotedString(),
                                                              true);
    }

    default: {
        if (curChar().isNumber() || curChar() == '-' || curChar() == '.') {
            return parseNumericTag();
        } else if (const auto &boolean = brigadier_bool(); boolean->isVaild()) {
            return QSharedPointer<Command::NbtByteNode>::create(
                startPos, boolean->length(), boolean->value());
        } else {
            const QString value = getWithCharset("a-zA-Z0-9-_.");
            if (value.isEmpty())
                error("Invaild empty tag value");
            return QSharedPointer<Command::NbtStringNode>::create(
                startPos, value);
        }
    }
    }
    return nullptr;
}

QSharedPointer<Command::NbtNode> Command::MinecraftParser::parseNumericTag() {
    auto number = brigadier_double();

    const QStringRef literal = text().midRef(number->pos(),
                                             number->length());
    bool ok = false;

    switch (curChar().toLower().toLatin1()) {
    case 'b': {
        advance();
        float integer = literal.toFloat(&ok);
        if (ok && integer <= 128 && integer >= -127)
            return QSharedPointer<Command::NbtByteNode>::create(number->pos(),
                                                                number->length(),
                                                                (int8_t)integer);
        else
            error(QString("%1 is not a vaild SNBT byte tag").arg(
                      literal));
    }

    case 'd': {
        advance();
        return QSharedPointer<Command::NbtDoubleNode>::create(number->pos(),
                                                              number->length(),
                                                              number->value());
    }

    case 'f': {
        advance();
        float value = literal.toFloat(&ok);
        if (ok)
            return QSharedPointer<Command::NbtFloatNode>::create(number->pos(),
                                                                 number->length(),
                                                                 value);
        else
            error(QString("%1 is not a vaild SNBT float tag").arg(
                      literal));
    }

    case 'l': {
        advance();
        int64_t value = literal.toLongLong(&ok);
        if (ok)
            return QSharedPointer<Command::NbtLongNode>::create(number->pos(),
                                                                number->length(),
                                                                value);
        else
            error(QString("%1 is not a vaild SNBT long tag").arg(
                      literal));
    }

    case 's': {
        advance();
        short value = literal.toFloat(&ok);
        if (ok)
            return QSharedPointer<Command::NbtShortNode>::create(number->pos(),
                                                                 number->length(),
                                                                 value);
        else
            error(QString("%1 is not a vaild SNBT short tag").arg(
                      literal));
    }

    default: {
        if (literal.contains('.')) {
            return QSharedPointer<Command::NbtDoubleNode>::create(number->pos(),
                                                                  number->length(),
                                                                  number->value());
        } else {
            int value = literal.toInt(&ok);
            if (ok)
                return QSharedPointer<Command::NbtIntNode>::create(
                    number->pos(),
                    number->length(),
                    value);
            else
                error(QString("%1 is not a vaild SNBT integer tag").arg(
                          literal));
        }
    }
    }
}

QSharedPointer<Command::NbtListNode> Command::MinecraftParser::parseListTag() {
    auto ret   = QSharedPointer<Command::NbtListNode>::create(pos() - 1);
    bool first = true;

    while (curChar() != ']') {
        skipWs(false);
        auto elem = parseTagValue();
        if (first || (elem->id() == ret->prefix())) {
            ret->append(elem);
            first = false;
        } else {
            error(QString("Type of elements in a list tag must be the same"));
        }
        skipWs(false);
        if (this->curChar() != ']') {
            this->eat(',');
            skipWs(false);
        }
    }
    eat(']');
    return ret;
}

QSharedPointer<Command::MapNode> Command::MinecraftParser::
parseEntityAdvancements() {
    return this->parseMap<MapNode, ParseNode>('{',
                                              '}',
                                              '=',
                                              [this](
                                                  const QString &key) -> QSharedPointer<ParseNode> {
        if (this->curChar() == '{') {
            return parseMap<MapNode, BoolNode>('{', '}', '=', [this](
                                                   const QString &key) ->
                                               QSharedPointer<Command::BoolNode>
            {
                auto ret = brigadier_bool();
                if (ret->isVaild())
                    return ret;
                else
                    this->error(QString("Argument value must be boolean"));
                return nullptr;
            }, false, R"(a-zA-z0-9-_:/)");
        } else {
            auto ret = brigadier_bool();
            if (ret->isVaild())
                return ret;
            else
                this->error(QString("Argument value must be boolean"));
            return nullptr;
        }
        return nullptr;
    },
                                              false,
                                              R"(a-zA-z0-9-_:/)");
}



QSharedPointer<Command::MultiMapNode> Command::MinecraftParser::
parseEntityArguments() {
    return this->parseMap<MultiMapNode, ParseNode>(
        '[', ']', '=', [this](
            const QString &key) -> QSharedPointer<ParseNode> {
        const QStringList doubleValKeys = { "x", "y", "z", "dx", "dy", "dz" };
        const QStringList rangeValKeys  = { "distance", "x_rotation", "y_rotation", "level" };
        if (doubleValKeys.contains(key)) {
            return brigadier_double();
        } else if (rangeValKeys.contains(key)) {
            return minecraft_floatRange();
        } else if (key == "limit") {
            return brigadier_integer();
        } else if (key == "predicate") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            return QSharedPointer<EntityArgumentValueNode>::create(
                minecraft_resourceLocation(),
                isNegative);
        } else if (key == "sort") {
            int curPos      = pos();
            QString literal = this->oneOf({ "nearest", "furthest",
                                            "random", "arbitrary" });
            return QSharedPointer<Command::StringNode>::create(curPos, literal);
        } else if (key == "gamemode") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos      = pos();
            QString literal = this->oneOf({ "adventure", "creative",
                                            "spectator", "survival" });
            return QSharedPointer<EntityArgumentValueNode>::create(
                QSharedPointer<Command::StringNode>::create(curPos, literal),
                isNegative);
        } else if (key == "name") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos = pos();
            QString literal;
            bool isQuote = false;
            if (curChar() == '"' || curChar() == '\'') {
                literal = getQuotedString();
                isQuote = true;
            } else {
                literal = this->getWithCharset("0-9a-zA-Z-_");
            }
            return QSharedPointer<EntityArgumentValueNode>::create(
                QSharedPointer<Command::StringNode>::create(
                    curPos, literal, isQuote), isNegative);
        } else if (key == "type") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            bool isTag = curChar() == '#';
            if (isTag)
                advance();
            auto resLoc = minecraft_resourceLocation();
            resLoc->setIsTag(isTag);
            return QSharedPointer<EntityArgumentValueNode>::create(resLoc,
                                                                   isNegative);
        } else if (key == "nbt") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            return QSharedPointer<EntityArgumentValueNode>::create(
                parseCompoundTag(),
                isNegative);
        } else if (key == "tag" || key == "team") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos      = pos();
            QString literal = getWithCharset("0-9a-zA-Z-_.+");
            return QSharedPointer<EntityArgumentValueNode>::create(
                QSharedPointer<Command::StringNode>
                ::create(
                    curPos, literal), isNegative);
        } else if (key == "scores") {
            return parseMap<MapNode, IntRangeNode>('{', '}', '=',
                                                   [this](const QString &key)
                                                   -> QSharedPointer<IntRangeNode>
            {
                return minecraft_intRange();
            });
        } else if (key == "advancements") {
            return parseEntityAdvancements();
        } else{
            this->error("Unknown entity argument name: '" + key + "'");
        }
    });
}

QSharedPointer<Command::TargetSelectorNode> Command::MinecraftParser::
parseTargetSelector() {
    auto ret = QSharedPointer<Command::TargetSelectorNode>::create(pos());

    eat('@');
    using Variable = Command::TargetSelectorNode::Variable;
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
        error(QString("Invaild target selector variable"));
    }
    }
    advance();
    if (curChar() == '[')
        ret->setArgs(parseEntityArguments());
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::NbtPathStepNode> Command::MinecraftParser::
parseNbtPathStep() {
    using StepType = Command::NbtPathStepNode::Type;
    auto ret = QSharedPointer<Command::NbtPathStepNode>::create(pos());

    switch (curChar().toLatin1()) {
    case '"': {
        ret->setName(QSharedPointer<StringNode>::create(ret->pos(),
                                                        getQuotedString(),
                                                        true));
        if (curChar() == '{')
            ret->setFilter(minecraft_nbtCompoundTag());
        break;
    }

    case '{': {
        ret->setType(StepType::Root);
        ret->setFilter(minecraft_nbtCompoundTag());
        break;
    }

    case '[': {
        ret->setType(StepType::Index);
        advance();
        skipWs(false);
        if (curChar() == ']') {
            /* Selects all. Continues. */
        } else if (curChar() == '{') {
            ret->setFilter(minecraft_nbtCompoundTag());
        } else {
            ret->setIndex(brigadier_integer());
            /*qDebug() << ret->index()->toString(); */
        }
        skipWs(false);
        eat(']');
        break;
    }

    default: {
        ret->setName(QSharedPointer<Command::StringNode>::create(ret->pos(),
                                                                 getWithCharset(
                                                                     "a-zA-Z0-9_")));
        /*qDebug() << "After key" << ret->name()->value(); */
        if (ret->name()->value().isEmpty())
            error(QString("Invaild NBT path key"));
        if (curChar() == '{')
            ret->setFilter(minecraft_nbtCompoundTag());
    };
    }
    /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
    if (curChar() == '.') {
        advance();
        ret->setHasTrailingDot(true);
    }
    /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
    ret->setLength(pos() - ret->pos());
    /*qDebug() << ret->toString() << ret->index(); */
    return ret;
}

QSharedPointer<Command::AngleNode> Command::MinecraftParser::minecraft_angle(
    const QVariantMap &props) {
    bool       _    = false;
    const auto axis = parseAxis(AxisParseOption::NoOption, _);

    return QSharedPointer<Command::AngleNode>::create(axis.get());
}

QSharedPointer<Command::BlockPosNode> Command::MinecraftParser::
minecraft_blockPos(const QVariantMap &props) {
    const auto axes = parseAxes(
        AxisParseOption::ParseY | AxisParseOption::CanBeLocal);
    auto ret = QSharedPointer<Command::BlockPosNode>::create(axes->pos(),
                                                             axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
    return ret;
}

QSharedPointer<Command::BlockStateNode> Command::MinecraftParser::
minecraft_blockState(const QVariantMap &props) {
    auto nspacedId = minecraft_resourceLocation();
    auto ret       = QSharedPointer<Command::BlockStateNode>::create(
        nspacedId->pos(),
        nspacedId->nspace(),
        nspacedId->id());

    if (this->curChar() == '[') {
        ret->setStates(
            parseMap<MapNode, ParseNode>('[', ']', '=',
                                         [this](const QString &key) {
            return brigadier_string({ { "type", "word" } });
        }));
    }
    if (this->curChar() == '{') {
        ret->setNbt(parseCompoundTag());
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::BlockPredicateNode> Command::MinecraftParser::
minecraft_blockPredicate(
    const QVariantMap &props) {
    bool isTag = false;

    if (curChar() == '#') {
        isTag = true;
        advance();
    }
    const auto blockState = minecraft_blockState();
    const auto ret        = QSharedPointer<BlockPredicateNode>::create(
        blockState.get());
    ret->setIsTag(isTag);
    ret->setPos(ret->pos() - isTag);
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::ColorNode> Command::MinecraftParser::minecraft_color(
    const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf({
        "aqua", "black", "blue",
        "dark_aqua", "dark_blue",
        "dark_green", "dark_grey",
        "dark_purple", "dark_red",
        "gold", "green", "grey",
        "light_purple", "red",
        "reset", "white", "yellow", });

    return QSharedPointer<Command::ColorNode>::create(curPos, literal);
}

QSharedPointer<Command::ColumnPosNode> Command::MinecraftParser::
minecraft_columnPos(const QVariantMap &props) {
    const auto axes = parseAxes(AxisParseOption::OnlyInteger |
                                AxisParseOption::
                                CanBeLocal);
    auto ret = QSharedPointer<Command::ColumnPosNode>::create(axes->pos(),
                                                              axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

QSharedPointer<Command::ComponentNode> Command::MinecraftParser::
minecraft_component(const QVariantMap &props) {
    int     curPos  = pos();
    QString literal = getUntil(QChar());

    try {
        json j = json::parse(
            literal.toStdString());
        QVariant vari;
        auto     ret =
            QSharedPointer<Command::ComponentNode>::create(curPos,
                                                           pos() - curPos);
        ret->setValue(j);
        return ret;
    }  catch (const json::parse_error &err) {
        error(err.what(), {}, curPos + err.byte - 1);
    }
    return nullptr;
}

QSharedPointer<Command::DimensionNode> Command::MinecraftParser::
minecraft_dimension(const QVariantMap &props) {
    const auto node = minecraft_resourceLocation();

    return QSharedPointer<Command::DimensionNode>::create(node->pos(),
                                                          node->nspace(),
                                                          node->id());
}

QSharedPointer<Command::EntityNode> Command::MinecraftParser::minecraft_entity(
    const QVariantMap &props) {
    QSharedPointer<EntityNode> ret    = nullptr;
    int                        curPos = pos();

    if (this->curChar() == '@') {
        ret = QSharedPointer<Command::EntityNode>::create(
            parseTargetSelector());
    } else {
        auto uuid = minecraft_uuid();
        if (uuid->isVaild()) {
            ret = QSharedPointer<Command::EntityNode>::create(uuid);
        } else {
            setPos(curPos);
            QString literal;
            literal = getWithCharset((props.contains(
                                          "charset")) ? props["charset"].toString() : "0-9a-zA-Z_");
            if (literal.isEmpty()) {
                this->error(QString("Invaild empty player name"));
            }
            auto playerName = QSharedPointer<Command::StringNode>::create(
                curPos,
                literal);
            ret = QSharedPointer<Command::EntityNode>::create(playerName);
        }
    }
    if (!ret)
        error(QString("Cannot parse entity"));
    return ret;
}

QSharedPointer<Command::EntityAnchorNode> Command::MinecraftParser::
minecraft_entityAnchor(const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf({ "eyes", "feet", });

    return QSharedPointer<Command::EntityAnchorNode>::create(curPos, literal);
}

QSharedPointer<Command::EntitySummonNode> Command::MinecraftParser::
minecraft_entitySummon(const QVariantMap &props) {
    const auto node = minecraft_resourceLocation();

    return QSharedPointer<Command::EntitySummonNode>::create(node->pos(),
                                                             node->nspace(),
                                                             node->id());
}

QSharedPointer<Command::FloatRangeNode> Command::MinecraftParser::
minecraft_floatRange(const QVariantMap &props) {
    auto ret    = QSharedPointer<Command::FloatRangeNode>::create(pos(), -1);
    bool hasMax = false;

    if (this->peek(2) == "..") {
        hasMax = true;
        this->advance(2);
    }
    auto num1 = this->brigadier_float();
    if (!hasMax) {
        bool hasDoubleDot = false;
        if (this->peek(2) == "..") {
            this->advance(2);
            hasDoubleDot = true;
        } else if ((text()[pos() - 1] == '.') && (curChar() == '.')) {
            this->advance();
            hasDoubleDot = true;
        }
        if (hasDoubleDot) {
            if (curChar().isDigit() || curChar() == '.' || curChar() == '-') {
                ret->setMaxValue(this->brigadier_float(), true);
            }
            ret->setMinValue(num1, true);
        } else {
            ret->setExactValue(num1);
        }
    } else {
        ret->setMaxValue(num1, false);
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::FunctionNode> Command::MinecraftParser::
minecraft_function(const QVariantMap &props) {
    bool isTag = curChar() == '#';

    if (isTag) {
        advance();
    }

    const auto node = minecraft_resourceLocation();
    auto       ret  = QSharedPointer<Command::FunctionNode>::create(node->pos(),
                                                                    node->nspace(),
                                                                    node->id());
    ret->setIsTag(isTag);
    return ret;
}

QSharedPointer<Command::GameProfileNode> Command::MinecraftParser::
minecraft_gameProfile(const QVariantMap &props) {
    const auto entity = minecraft_entity(props);

    return QSharedPointer<Command::GameProfileNode>::create(entity.get());
}

QSharedPointer<Command::IntRangeNode> Command::MinecraftParser::
minecraft_intRange(const QVariantMap &props) {
    auto ret    = QSharedPointer<Command::IntRangeNode>::create(pos(), -1);
    bool hasMax = false;

    if (this->peek(2) == "..") {
        hasMax = true;
        this->advance(2);
    }
    auto num1 = this->brigadier_integer();
    if (!hasMax) {
        if (this->peek(2) == "..") {
            this->advance(2);
            if (curChar().isDigit() || curChar() == '-') {
                ret->setMaxValue(brigadier_integer(), true);
            }
            ret->setMinValue(num1, true);
        } else {
            ret->setExactValue(num1);
        }
    } else {
        ret->setMaxValue(num1, false);
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::ItemEnchantmentNode> Command::MinecraftParser::
minecraft_itemEnchantment(const QVariantMap &props) {
    const auto node = minecraft_resourceLocation();

    return QSharedPointer<Command::ItemEnchantmentNode>::create(node->pos(),
                                                                node->nspace(),
                                                                node->id());
}

QSharedPointer<Command::ItemSlotNode> Command::MinecraftParser::
minecraft_itemSlot(const QVariantMap &props) {
    int           curPos  = pos();
    const QString objname = this->getWithCharset("a-z0-9._");

    return QSharedPointer<Command::ItemSlotNode>::create(curPos, objname);
}

QSharedPointer<Command::ItemStackNode> Command::MinecraftParser::
minecraft_itemStack(const QVariantMap &props) {
    const auto nspacedId = minecraft_resourceLocation();
    auto       ret       = QSharedPointer<ItemStackNode>::create(
        nspacedId->pos(), nspacedId->nspace(), nspacedId->id());

    if (this->curChar() == '{') {
        ret->setNbt(parseCompoundTag());
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::ItemPredicateNode> Command::MinecraftParser::
minecraft_itemPredicate(const QVariantMap &props) {
    bool isTag = false;

    if (curChar() == '#') {
        isTag = true;
        advance();
    }
    auto itemStack = minecraft_itemStack();
    auto ret       = QSharedPointer<ItemPredicateNode>::create(itemStack.get());
    ret->setIsTag(isTag);
    ret->setPos(ret->pos() - isTag);
    return ret;
}

QSharedPointer<Command::MessageNode> Command::MinecraftParser::minecraft_message(
    const QVariantMap &props) {
    const auto &str = brigadier_string({ { "type", "greedy" } });

    return QSharedPointer<Command::MessageNode>::create(str->pos(),
                                                        str->value());
}

QSharedPointer<Command::MobEffectNode> Command::MinecraftParser::
minecraft_mobEffect(const QVariantMap &props) {
    const auto node = minecraft_resourceLocation();

    return QSharedPointer<Command::MobEffectNode>::create(node->pos(),
                                                          node->nspace(),
                                                          node->id());
}

QSharedPointer<Command::NbtCompoundNode> Command::MinecraftParser::
minecraft_nbtCompoundTag(const QVariantMap &props) {
    return parseCompoundTag();
}

QSharedPointer<Command::NbtPathNode> Command::MinecraftParser::minecraft_nbtPath(
    const QVariantMap &props) {
    auto ret = QSharedPointer<Command::NbtPathNode>::create(pos());

    ret->append(parseNbtPathStep());
    auto last = ret->last();
    while (last->hasTrailingDot() || curChar() == '[' || curChar() == '"') {
        auto step = parseNbtPathStep();
/*
          qDebug() << QMetaEnum::fromType<NbtPathStepNode::Type>().valueToKey(
              static_cast<int>(last->type())) << last->hasTrailingDot();
 */
        if ((step->type() == Command::NbtPathStepNode::Type::Key)
            && (!last->hasTrailingDot())) {
            error("Missing character '.' before a named tag", {},
                  last->pos() + last->length() - 1);
        }
        ret->append(step);
        last = ret->last();
        /*qDebug() << curChar() << curChar().isNull() << last->hasTrailingDot(); */
        if (curChar().isNull())
            break;
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::NbtNode> Command::MinecraftParser::minecraft_nbtTag(
    const QVariantMap &props) {
    return parseTagValue();
}

QSharedPointer<Command::ObjectiveNode> Command::MinecraftParser::
minecraft_objective(
    const QVariantMap &props) {
    int     curPos  = pos();
    QString objname = this->getWithCharset("0-9a-zA-Z-+_.#");

    if (objname.length() > 16)
        this->error("objective '%1' must be less than 16 characters",
                    { objname },
                    curPos,
                    objname.length());
    return QSharedPointer<Command::ObjectiveNode>::create(curPos, objname);
}

QSharedPointer<Command::ObjectiveCriteriaNode> Command::MinecraftParser::
minecraft_objectiveCriteria(const QVariantMap &props) {
    int     curPos   = pos();
    QString criteria = this->getWithCharset("-\\w.:");

    return QSharedPointer<Command::ObjectiveCriteriaNode>::create(curPos,
                                                                  criteria);
}

QSharedPointer<Command::OperationNode> Command::MinecraftParser::
minecraft_operation(
    const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf(
        { "=", "<", ">", "><", "+=", "-=", "*=", "/=", "%=", });

    return QSharedPointer<Command::OperationNode>::create(curPos, literal);
}

QSharedPointer<Command::ParticleNode> Command::MinecraftParser::
minecraft_particle(
    const QVariantMap &props) {
    auto    nspacedId = minecraft_resourceLocation();
    auto    ret       = QSharedPointer<ParticleNode>::create(nspacedId.get());
    QString fullid    = ret->fullId();

    if (fullid == "minecraft:block" || fullid == "minecraft:falling_dust") {
        eat(' ');
        ret->setParams(minecraft_blockState());
    } else if (fullid == "minecraft:dust") {
        auto color = QSharedPointer<ParticleColorNode>::create(pos());
        this->eat(' ');
        color->setR(brigadier_float());
        this->eat(' ');
        color->setG(brigadier_float());
        this->eat(' ');
        color->setB(brigadier_float());
        this->eat(' ');
        color->setSize(brigadier_float());
        color->setLength(pos() - color->pos());
        ret->setParams(color);
    } else if (fullid == "minecraft:item") {
        eat(' ');
        ret->setParams(minecraft_itemStack());
    }
    ret->setLength(pos() - ret->pos());
    return ret;
}

QSharedPointer<Command::ResourceLocationNode> Command::MinecraftParser::
minecraft_resourceLocation(const QVariantMap &props) {
    const int     curPos  = pos();
    QString       nspace  = "";
    QString       id      = "";
    const QString charset = QStringLiteral("0-9a-z-_\\/.");

    id = this->getWithCharset(charset);
    if (this->curChar() == ':') {
        this->advance();
        nspace = id;
        id     = this->getWithCharset(charset);
    }
    if (nspace.contains('/'))
        this->error(QStringLiteral(
                        "character '/' is not allowed in the namespace"));
    if (id.isEmpty())
        this->error(QStringLiteral("Invaild empty namespaced ID"));

    auto ret = QSharedPointer<ResourceLocationNode>::create(curPos, nspace, id);
    return ret;
}

QSharedPointer<Command::RotationNode> Command::MinecraftParser::
minecraft_rotation(
    const QVariantMap &props) {
    const auto axes = parseAxes(AxisParseOption::NoOption);
    auto       ret  = QSharedPointer<RotationNode>::create(axes->pos(),
                                                           axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

QSharedPointer<Command::ScoreHolderNode> Command::MinecraftParser::
minecraft_scoreHolder(
    const QVariantMap &props) {
    auto ret = QSharedPointer<Command::ScoreHolderNode>::create(pos());

    if (curChar() == '*') {
        ret->setAll(true);
        advance();
    } else {
        QVariantMap newProps(props);
        newProps.insert("charset", "0-9a-zA-Z-_#$.");
        auto entity = minecraft_entity(newProps);
        ret = QSharedPointer<Command::ScoreHolderNode>::create(entity.get());
    }
    return ret;
}

QSharedPointer<Command::ScoreboardSlotNode> Command::MinecraftParser::
minecraft_scoreboardSlot(
    const QVariantMap &props) {
    int     curPos = pos();
    QString slot   = this->oneOf({ "list", "sidebar.team",
                                   "sidebar", "belowName" });

    if (slot == "sidebar.team") {
        this->eat('.');
        auto color = minecraft_color();
        slot += '.' + color->value();
    }
    return QSharedPointer<Command::ScoreboardSlotNode>::create(curPos, slot);
}

QSharedPointer<Command::SwizzleNode> Command::MinecraftParser::minecraft_swizzle(
    const QVariantMap &props) {
    auto ret = QSharedPointer<Command::SwizzleNode>::create(pos(), false, false,
                                                            false);
    QString acceptedChars = "xyz";

    while (acceptedChars.contains(this->curChar())) {
        switch (curChar().toLatin1()) {
        case 'x': {
            ret->setAxes(ret->axes() | Command::SwizzleNode::Axis::X);
            break;
        }

        case 'y': {
            ret->setAxes(ret->axes() | Command::SwizzleNode::Axis::Y);
            break;
        }

        case 'z': {
            ret->setAxes(ret->axes() | Command::SwizzleNode::Axis::Z);
            break;
        }
        }
        acceptedChars = acceptedChars.replace(this->curChar(), "");
        this->advance();
    }
    this->expect(' ', true);
    return ret;
}

QSharedPointer<Command::TeamNode> Command::MinecraftParser::minecraft_team(
    const QVariantMap &props) {
    int     curPos  = pos();
    QString literal = this->getWithCharset(QStringLiteral("-\\w.:"));

    return QSharedPointer<Command::TeamNode>::create(curPos, literal);
}

QSharedPointer<Command::TimeNode> Command::MinecraftParser::minecraft_time(
    const QVariantMap &props) {
    int  curPos = pos();
    auto unit   = Command::TimeNode::Unit::Tick;
    auto number = brigadier_float();

    switch (this->curChar().toLatin1()) {
    case 'd': {
        unit = Command::TimeNode::Unit::Day;
        advance();
        break;
    }

    case 's': {
        unit = Command::TimeNode::Unit::Second;
        advance();
        break;
    }

    case 't': {
        unit = Command::TimeNode::Unit::Tick;
        advance();
        break;
    }
    }
    return QSharedPointer<Command::TimeNode>::create(curPos, pos() - curPos,
                                                     number->value(), unit);
}

QSharedPointer<Command::UuidNode> Command::MinecraftParser::minecraft_uuid(
    const QVariantMap &props) {
    int     curPos = pos();
    QString uuid   = getWithRegex(QStringLiteral(
                                      R"([\da-fA-F]{8}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{12})"));

    return QSharedPointer<Command::UuidNode>::create(curPos, uuid);
}

QSharedPointer<Command::Vec2Node> Command::MinecraftParser::minecraft_vec2(
    const QVariantMap &props) {
    const auto axes = parseAxes(AxisParseOption::CanBeLocal);
    auto       ret  = QSharedPointer<Command::Vec2Node>::create(axes->pos(),
                                                                axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

QSharedPointer<Command::Vec3Node> Command::MinecraftParser::minecraft_vec3(
    const QVariantMap &props) {
    const auto axes = parseAxes(AxisParseOption::CanBeLocal
                                | AxisParseOption::ParseY);
    auto ret = QSharedPointer<Command::Vec3Node>::create(axes->pos(),
                                                         axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
    return ret;
}
