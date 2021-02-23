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
        qDebug() << obj;
    });
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

Command::AxisNode *Command::MinecraftParser::parseAxis(QObject *parent,
                                                       AxisParseOptions options,
                                                       bool &isLocal) {
    auto      *axis           = new Command::AxisNode(parent);
    const bool canBeLocal     = options & AxisParseOption::CanBeLocal;
    const bool onlyInt        = options & AxisParseOption::OnlyInteger;
    QString    mixCoordErrMsg =
        "Cannot mix world & local coordinates (everything must either use ^ or not).";
    const auto axisVal = [ =, &isLocal](Command::AxisNode *axis) {
                             if (onlyInt) {
                                 QScopedPointer<Command::IntegerNode> intNode(
                                     brigadier_integer(nullptr));
                                 axis->setValue(intNode->value());
                             } else {
                                 QScopedPointer<Command::FloatNode> floatNode(
                                     brigadier_float(nullptr));
                                 axis->setValue(floatNode->value());
                             }
                         };

    axis->setPos(pos());
    bool hasPrefix = false;
    if (this->curChar() == '~') {
        if (!isLocal) {
            axis->setType(Command::AxisNode::AxisType::Relative);
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
            axis->setType(Command::AxisNode::AxisType::Local);
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

Command::AxesNode *Command::MinecraftParser::parseAxes(QObject *parent,
                                                       AxisParseOptions options)
{
    bool                     isLocal = false;
    QScopedPointer<AxesNode> axes(new Command::AxesNode(this, pos()));

    axes->setX(parseAxis(axes.get(), options, isLocal));
    if (options & AxisParseOption::ParseY) {
        this->eat(' ');
        axes->setY(parseAxis(axes.get(), options, isLocal));
        axes->z();
    }

    this->eat(' ');
    axes->setZ(parseAxis(axes.get(), options, isLocal));
    return axes.take();
}

Command::NbtCompoundNode *Command::MinecraftParser::parseCompoundTag(
    QObject *parent) {
    return parseMap<NbtCompoundNode, NbtNode>(
        parent, '{', '}', ':', [this, parent](const QString &key)
        -> NbtNode* {
        return parseTagValue(parent);
    }, true);
}

Command::NbtNode *Command::MinecraftParser::parseTagValue(QObject *parent) {
    switch (curChar().toLatin1()) {
    case '{': {
        return parseCompoundTag(parent);
    }

    case '[': {
        advance();
        switch (curChar().toLower().toLatin1()) {
        case 'b': {
            return parseArrayTag<NbtByteArrayNode, NbtByteNode>(
                parent, "Unvaild SNBT tag in a byte array tag");
        }

        case 'i': {
            return parseArrayTag<NbtIntArrayNode, NbtIntNode>(
                parent, "Unvaild SNBT tag in an integer array tag");
        }

        case 'l': {
            return parseArrayTag<NbtLongArrayNode, NbtLongNode>(
                parent, "Unvaild SNBT tag in a long array tag");
        }

        default: {
            return parseListTag(parent);
        }
        }
    }

    case '"':
    case '\'': {
        return new Command::NbtStringNode(parent, pos(), getQuotedString());
    }

    default: {
        if (curChar().isNumber() || curChar() == '-' || curChar() == '.') {
            parseNumericTag(parent);
        } else {
            return new Command::NbtStringNode(parent, pos(),
                                              getWithCharset("a-zA-Z-_."));
        }
    }
    }
}

Command::NbtNode *Command::MinecraftParser::parseNumericTag(QObject *parent) {
    QScopedPointer<DoubleNode> number(brigadier_double(parent));

    const QStringRef literal = text().midRef(number->pos(),
                                             number->length());
    bool ok = false;

    switch (curChar().toLower().toLatin1()) {
    case 'b': {
        advance();
        float integer = literal.toFloat(&ok);
        if (ok && integer <= 128 && integer >= -127)
            return new NbtByteNode(parent, number->pos(),
                                   number->length(), (int8_t)integer);
        else
            error(QString("%1 is not a vaild SNBT byte tag").arg(
                      literal));
    }

    case 'd': {
        advance();
        return new NbtDoubleNode(parent, number->pos(),
                                 number->length(), number->value());
    }

    case 'f': {
        advance();
        float value = literal.toFloat(&ok);
        if (ok)
            return new NbtFloatNode(parent, number->pos(),
                                    number->length(), value);
        else
            error(QString("%1 is not a vaild SNBT float tag").arg(
                      literal));
    }

    case 'l': {
        advance();
        int64_t value = literal.toLongLong(&ok);
        if (ok)
            return new NbtLongNode(parent, number->pos(),
                                   number->length(), value);
        else
            error(QString("%1 is not a vaild SNBT long tag").arg(
                      literal));
    }

    case 's': {
        advance();
        short value = literal.toFloat(&ok);
        if (ok)
            return new NbtShortNode(parent, number->pos(),
                                    number->length(), value);
        else
            error(QString("%1 is not a vaild SNBT short tag").arg(
                      literal));
    }

    default: {
        if (literal.contains('.')) {
            return new NbtDoubleNode(parent, number->pos(),
                                     number->length(), number->value());
        } else {
            int value = literal.toInt(&ok);
            if (ok)
                return new NbtIntNode(parent, number->pos(),
                                      number->length(), value);
            else
                error(QString("%1 is not a vaild SNBT integer tag").arg(
                          literal));
        }
    }
    }
}

Command::NbtListNode *Command::MinecraftParser::parseListTag(QObject *parent) {
    QScopedPointer<NbtListNode> ret(new NbtListNode(parent, pos() - 1));
    bool                        first = true;

    while (curChar() != ']') {
        skipWs(false);
        QScopedPointer<NbtNode> elem(parseTagValue(parent));
        if (first || (elem->id() == ret->prefix())) {
            ret->append(elem.take());
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
    return ret.take();
}

Command::MapNode *Command::MinecraftParser::parseEntityAdvancements(
    QObject *parent) {
    return this->parseMap<MapNode, ParseNode>(
        parent, '{', '}', '=', [this, parent](const QString &key) -> ParseNode*{
        if (this->curChar() == '{') {
            return parseMap<MapNode, BoolNode>(parent, '{', '}', '=',
                                               [this, parent](
                                                   const QString &key) {
                QScopedPointer<BoolNode> ret(brigadier_bool(parent));
                if (ret->isVaild())
                    return ret.take();
                else
                    this->error(QString("Argument value must be boolean"));
            }, false, R"(a-zA-z0-9-_:/)");
        } else {
            QScopedPointer<BoolNode> ret(brigadier_bool(parent));
            if (ret->isVaild())
                return ret.take();
            else
                this->error(QString("Argument value must be boolean"));
        }
    }, false, R"(a-zA-z0-9-_:/)");
}



Command::MapNode *Command::MinecraftParser::parseEntityArguments(
    QObject *parent) {
    return this->parseMap<MapNode, ParseNode>(
        parent, '[', ']', '=', [this, parent](
            const QString &key) -> ParseNode*{
        const QStringList doubleValKeys = { "x", "y", "z", "dx", "dy", "dz" };
        const QStringList rangeValKeys  = { "distance", "x_rotation", "y_rotation", "level" };
        if (doubleValKeys.contains(key)) {
            return brigadier_double(parent);
        } else if (rangeValKeys.contains(key)) {
            return minecraft_floatRange(parent);
        } else if (key == "limit") {
            return brigadier_integer(parent);
        } else if (key == "predicate") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            return new EntityArgumentValueNode(minecraft_resourceLocation(parent),
                                               isNegative);
        } else if (key == "sort") {
            int curPos      = pos();
            QString literal = this->oneOf({ "nearest", "furthest",
                                            "random", "arbitrary" });
            return new Command::StringNode(parent, curPos, literal);
        } else if (key == "gamemode") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos      = pos();
            QString literal = this->oneOf({ "adventure", "creative",
                                            "spectator", "survival" });
            return new EntityArgumentValueNode(new StringNode(parent, curPos,
                                                              literal),
                                               isNegative);
        } else if (key == "name") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos = pos();
            QString literal;
            if (curChar() == '"')
                literal = getQuotedString();
            else
                literal = this->getWithCharset("0-9a-zA-Z-_");
            return new EntityArgumentValueNode(new StringNode(parent, curPos,
                                                              literal),
                                               isNegative);
        } else if (key == "type") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            bool isTag = curChar() == '#';
            if (isTag)
                advance();
            QScopedArrayPointer<ResourceLocationNode> resLoc(
                minecraft_resourceLocation(parent));
            resLoc->setIsTag(isTag);
            return new EntityArgumentValueNode(resLoc.take(), isNegative);
        } else if (key == "nbt") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            return new EntityArgumentValueNode(parseCompoundTag(parent),
                                               isNegative);
        } else if (key == "tag" || key == "team") {
            bool isNegative = curChar() == '!';
            if (isNegative)
                advance();
            int curPos      = pos();
            QString literal = getWithCharset("0-9a-zA-Z-_.+");
            return new EntityArgumentValueNode(new StringNode(parent, curPos,
                                                              literal),
                                               isNegative);
        } else if (key == "scores") {
            return parseMap<MapNode, IntRangeNode>(parent, '{', '}', '=',
                                                   [this,
                                                    parent](const QString &key)
                                                   -> IntRangeNode*{
                return minecraft_intRange(parent);
            });
        } else if (key == "advancements") {
            return parseEntityAdvancements(parent);
        } else{
            this->error("Unknown entity argument name: '" + key + "'");
        }
    });
}

Command::TargetSelectorNode *Command::MinecraftParser::parseTargetSelector(
    QObject *parent) {
    QScopedPointer<TargetSelectorNode> ret(new TargetSelectorNode(parent,
                                                                  pos()));

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
        ret->setArgs(parseEntityArguments(ret.get()));
    return ret.take();
}

Command::NbtPathStepNode *Command::MinecraftParser::parseNbtPathStep(
    QObject *parent) {
    using StepType = Command::NbtPathStepNode::Type;
    QScopedPointer<NbtPathStepNode> ret(new NbtPathStepNode(parent, pos()));

    switch (curChar().toLatin1()) {
    case '"': {
        ret->setName(new StringNode(parent, pos(), getQuotedString()));
        if (curChar() == '{')
            ret->setFilter(minecraft_nbtCompoundTag(parent));
        break;
    }

    case '{': {
        ret->setType(StepType::Root);
        ret->setFilter(minecraft_nbtCompoundTag(parent));
        break;
    }

    case '[': {
        ret->setType(StepType::Index);
        advance();
        skipWs(false);
        if (curChar() == ']') {
            /* Selects all. Continues. */
        } else if (curChar() == '{') {
            ret->setFilter(minecraft_nbtCompoundTag(parent));
        } else {
            ret->setIndex(brigadier_integer(parent));
            /*qDebug() << ret->index()->toString(); */
        }
        skipWs(false);
        eat(']');
        break;
    }

    default: {
        ret->setName(new StringNode(parent, pos(),
                                    getWithCharset("a-zA-Z0-9_")));
        /*qDebug() << "After key" << ret->name()->value(); */
        if (ret->name()->value().isEmpty())
            error(QString("Invaild NBT pat key name"));
        if (curChar() == '{')
            ret->setFilter(minecraft_nbtCompoundTag(parent));
    };
    }
    /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
    if (curChar() == '.') {
        advance();
        ret->setHasTrailingDot(true);
    }
    /*qDebug() << "After step:" << curChar() << ret->hasTrailingDot(); */
    ret->setLength(pos() - ret->pos() + 1);
    /*qDebug() << ret->toString() << ret->index(); */
    return ret.take();
}

Command::BlockPosNode *Command::MinecraftParser::minecraft_blockPos(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<AxesNode> axes(parseAxes(parent,
                                                  AxisParseOption::ParseY |
                                                  AxisParseOption::CanBeLocal));
    auto *ret = new Command::BlockPosNode(parent, axes->pos(), axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
    return ret;
}

Command::BlockStateNode *Command::MinecraftParser::minecraft_blockState(
    QObject *parent, const QVariantMap &props) {
    QScopedPointer<Command::ResourceLocationNode> nspacedId(
        minecraft_resourceLocation(this));
    QScopedPointer<Command::BlockStateNode>
    ret(new Command::BlockStateNode(parent, nspacedId->pos(),
                                    nspacedId->nspace(),
                                    nspacedId->id()));
    auto *retPtr = ret.get();

    if (this->curChar() == '[') {
        ret->setStates(
            parseMap<MapNode, ParseNode>(ret.get(), '[', ']', '=',
                                         [this, retPtr]
                                             (const QString &key) {
            return brigadier_string(retPtr, { { "type", "word" } });
        }));
    }
    if (this->curChar() == '{') {
        ret->setNbt(parseCompoundTag(ret.get()));
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret.take();
}

Command::BlockPredicateNode *Command::MinecraftParser::minecraft_blockPredicate(
    QObject *parent, const QVariantMap &props) {
    bool isTag = false;

    if (curChar() == '#') {
        isTag = true;
        advance();
    }
    QScopedPointer<Command::BlockStateNode> blockState(
        minecraft_blockState(parent));
    auto *ret = new BlockPredicateNode(blockState.get());
    ret->setIsTag(isTag);
    return ret;
}

Command::ColorNode *Command::MinecraftParser::minecraft_color(
    QObject *parent, const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf({
        "aqua", "black", "blue",
        "dark_aqua", "dark_blue",
        "dark_green", "dark_grey",
        "dark_purple", "dark_red",
        "gold", "green", "grey",
        "light_purple", "red",
        "reset", "white", "yellow", });

    return new ColorNode(parent, curPos, literal);
}

Command::ColumnPosNode *Command::MinecraftParser::minecraft_columnPos(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::AxesNode> axes(parseAxes(parent,
                                                           AxisParseOption::
                                                           OnlyInteger |
                                                           AxisParseOption::
                                                           CanBeLocal));
    auto *ret = new Command::ColumnPosNode(parent, axes->pos(), axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

Command::ComponentNode *Command::MinecraftParser::minecraft_component(
    QObject *parent, const QVariantMap &props) {
    int     curPos  = pos();
    QString literal = getUntil(QChar());

    try {
        json j = json::parse(
            literal.toStdString());
        QVariant                               vari;
        QScopedPointer<Command::ComponentNode> ret(
            new ComponentNode(parent, curPos, pos() - curPos + 1));
        ret->setValue(j);
        return ret.take();
    }  catch (const json::parse_error &err) {
        error(QString(err.what()), curPos + err.byte - 1);
    }
}

Command::DimensionNode *Command::MinecraftParser::minecraft_dimension(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::DimensionNode(parent, node->pos(),
                                      node->nspace(), node->id());
}

Command::EntityNode *Command::MinecraftParser::minecraft_entity(QObject *parent,
                                                                const QVariantMap &props)
{
    QScopedPointer<EntityNode> ret(nullptr);
    int                        curPos = pos();

    if (this->curChar() == '@') {
        ret.reset(new EntityNode(parent, parseTargetSelector(this)));
    } else {
        QScopedPointer<UuidNode> uuid(minecraft_uuid(parent));
        if (uuid->isVaild()) {
            ret.reset(new EntityNode(parent, uuid.take()));
        } else {
            setPos(curPos);
            QString literal = getWithCharset((props.contains(
                                                  "charset")) ? props["charset"].toString() : "0-9a-zA-Z_");
            if (literal.isEmpty()) {
                this->error(QString("Invaild empty player name"));
            }
            QScopedPointer<StringNode> playerName(new StringNode(parent, curPos,
                                                                 literal));
            ret.reset(new EntityNode(parent, playerName.take()));
        }
    }
    if (!ret)
        error(QString("Cannot parse entity"));
    return ret.take();
}

Command::EntityAnchorNode *Command::MinecraftParser::minecraft_entityAnchor(
    QObject *parent, const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf({ "eyes", "feet", });

    return new EntityAnchorNode(parent, curPos, literal);
}

Command::EntitySummonNode *Command::MinecraftParser::minecraft_entitySummon(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::EntitySummonNode(parent, node->pos(),
                                         node->nspace(), node->id());
}

Command::FloatRangeNode *Command::MinecraftParser::minecraft_floatRange(
    QObject *parent, const QVariantMap &props) {
    auto *ret    = new Command::FloatRangeNode(parent, pos(), -1);
    bool  hasMax = false;

    if (this->peek(2) == "..") {
        hasMax = true;
        this->advance(2);
    }
    QScopedPointer<FloatNode> num1(this->brigadier_float(ret));
    if (!hasMax) {
        if (this->peek(2) == "..") {
            this->advance(2);
            if (curChar().isDigit() || curChar() == '.' || curChar() == '-') {
                ret->setMaxValue(this->brigadier_float(ret), true);
            }
            ret->setMinValue(num1.take(), true);
        } else {
            ret->setExactValue(num1.take());
        }
    } else {
        ret->setMaxValue(num1.take(), false);
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret;
}

Command::FunctionNode *Command::MinecraftParser::minecraft_function(
    QObject *parent, const QVariantMap &props) {
    bool isTag = curChar() == '#';

    if (isTag) {
        advance();
    }

    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));
    QScopedPointer<Command::FunctionNode> ret(
        new Command::FunctionNode(parent, node->pos(),
                                  node->nspace(), node->id()));
    ret->setIsTag(isTag);
    return ret.take();
}

Command::GameProfileNode *Command::MinecraftParser::minecraft_gameProfile(
    QObject *parent, const QVariantMap &props) {
    QScopedPointer<EntityNode> entity(minecraft_entity(parent, props));

    return new Command::GameProfileNode(entity.get());
}

Command::IntRangeNode *Command::MinecraftParser::minecraft_intRange(
    QObject *parent, const QVariantMap &props) {
    auto *ret    = new Command::IntRangeNode(parent, pos(), -1);
    bool  hasMax = false;

    if (this->peek(2) == "..") {
        hasMax = true;
        this->advance(2);
    }
    QScopedPointer<IntegerNode> num1(this->brigadier_integer(ret));
    if (!hasMax) {
        if (this->peek(2) == "..") {
            this->advance(2);
            if (curChar().isDigit() || curChar() == '-') {
                ret->setMaxValue(brigadier_integer(ret), true);
            }
            ret->setMinValue(num1.take(), true);
        } else {
            ret->setExactValue(num1.take());
        }
    } else {
        ret->setMaxValue(num1.take(), false);
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret;
}

Command::ItemEnchantmentNode *Command::MinecraftParser::
minecraft_itemEnchantment(QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::ItemEnchantmentNode(parent, node->pos(),
                                            node->nspace(), node->id());
}

Command::ItemSlotNode *Command::MinecraftParser::minecraft_itemSlot(
    QObject *parent, const QVariantMap &props) {
    int     curPos  = pos();
    QString objname = this->getWithCharset("a-z0-9._");

    return new Command::ItemSlotNode(parent, curPos, objname);
}

Command::ItemStackNode *Command::MinecraftParser::minecraft_itemStack(
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<Command::ResourceLocationNode> nspacedId(
        minecraft_resourceLocation(this));
    QScopedPointer<Command::ItemStackNode>
    ret(new Command::ItemStackNode(parent, nspacedId->pos(),
                                   nspacedId->nspace(),
                                   nspacedId->id()));

    if (this->curChar() == '{') {
        ret->setNbt(parseCompoundTag(ret.get()));
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret.take();
}

Command::ItemPredicateNode *Command::MinecraftParser::minecraft_itemPredicate(
    QObject *parent, const QVariantMap &props) {
    bool isTag = false;

    if (curChar() == '#') {
        isTag = true;
        advance();
    }
    QScopedPointer<Command::ItemStackNode> itemStack(
        minecraft_itemStack(parent));
    auto *ret = new ItemPredicateNode(itemStack.get());
    ret->setIsTag(isTag);
    return ret;
}

Command::MessageNode *Command::MinecraftParser::minecraft_message(
    QObject *parent, const QVariantMap &props) {
    QScopedPointer<Command::StringNode> str(brigadier_string(parent,
                                                             { { "type",
                                                                 "greedy" } }));

    return new MessageNode(parent, str->pos(), str->value());
}

Command::MobEffectNode *Command::MinecraftParser::minecraft_mobEffect(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::MobEffectNode(parent, node->pos(),
                                      node->nspace(), node->id());
}

Command::NbtCompoundNode *Command::MinecraftParser::minecraft_nbtCompoundTag(
    QObject *parent, const QVariantMap &props) {
    return parseCompoundTag(parent);
}

Command::NbtPathNode *Command::MinecraftParser::minecraft_nbtPath(
    QObject *parent, const QVariantMap &props) {
    QScopedPointer<Command::NbtPathNode> ret(new NbtPathNode(parent, pos()));

    ret->append(parseNbtPathStep(ret.get()));
    const auto *last = ret->last();
    while (last->hasTrailingDot() || curChar() == '[' || curChar() == '"') {
        auto *step = parseNbtPathStep(ret.get());
/*
          qDebug() << QMetaEnum::fromType<NbtPathStepNode::Type>().valueToKey(
              static_cast<int>(last->type())) << last->hasTrailingDot();
 */
        if ((step->type() == Command::NbtPathStepNode::Type::Key)
            && (!last->hasTrailingDot())) {
            delete step;
            error(QString("Missing character '.' before a named tag"),
                  last->pos() + last->length() - 1);
        }
        ret->append(step);
        last = ret->last();
        /*qDebug() << curChar() << curChar().isNull() << last->hasTrailingDot(); */
        if (curChar().isNull())
            break;
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret.take();
}

Command::NbtNode *Command::MinecraftParser::minecraft_nbtTag(QObject *parent,
                                                             const QVariantMap &props)
{
    return parseTagValue(parent);
}

Command::ObjectiveNode *Command::MinecraftParser::minecraft_objective(
    QObject *parent, const QVariantMap &props) {
    int     curPos  = pos();
    QString objname = this->getWithCharset("0-9a-zA-Z-+_.#");

    if (objname.length() > 16)
        this->error("objective " + objname + " must be less than 16 characters",
                    curPos, objname.length());
    return new Command::ObjectiveNode(parent, curPos, objname);
}

Command::ObjectiveCriteriaNode *Command::MinecraftParser::
minecraft_objectiveCriteria(QObject *parent, const QVariantMap &props) {
    int     curPos   = pos();
    QString criteria = this->getWithCharset("-\\w.:");

    return new Command::ObjectiveCriteriaNode(parent, curPos, criteria);
}

Command::OperationNode *Command::MinecraftParser::minecraft_operation(
    QObject *parent, const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf(
        { "=", "<", ">", "><", "+=", "-=", "*=", "/=", "%=", });

    return new OperationNode(parent, curPos, literal);
}

Command::ParticleNode *Command::MinecraftParser::minecraft_particle(
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<Command::ResourceLocationNode> nspacedId(
        minecraft_resourceLocation(this));
    QScopedPointer<Command::ParticleNode>
            ret(new Command::ParticleNode(nspacedId.get()));
    auto   *retPtr = ret.get();
    QString fullid = ret->fullId();

    if (fullid == "minecraft:block" || fullid == "minecraft:falling_dust") {
        eat(' ');
        ret->setParams(minecraft_blockState(retPtr));
    } else if (fullid == "minecraft:dust") {
        QScopedPointer<Command::ParticleColorNode>
              color(new Command::ParticleColorNode(retPtr, pos()));
        auto *colorPtr = color.get();
        this->eat(' ');
        color->setR(brigadier_float(colorPtr));
        this->eat(' ');
        color->setG(brigadier_float(colorPtr));
        this->eat(' ');
        color->setB(brigadier_float(colorPtr));
        this->eat(' ');
        color->setSize(brigadier_float(colorPtr));
        color->setLength(pos() - color->pos() + 1);
        ret->setParams(color.take());
    } else if (fullid == "minecraft:item") {
        eat(' ');
        ret->setParams(minecraft_itemStack(retPtr));
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret.take();
}

Command::ResourceLocationNode *Command::MinecraftParser::
minecraft_resourceLocation(QObject *parent, const QVariantMap &props) {
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

    auto *ret = new Command::ResourceLocationNode(parent, curPos, nspace, id);
    return ret;
}

Command::RotationNode *Command::MinecraftParser::minecraft_rotation(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::AxesNode> axes(parseAxes(parent,
                                                           AxisParseOption::
                                                           NoOption));
    auto *ret = new Command::RotationNode(parent, axes->pos(),
                                          axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

Command::ScoreHolderNode *Command::MinecraftParser::minecraft_scoreHolder(
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<ScoreHolderNode> ret(new ScoreHolderNode(parent, pos()));

    if (curChar() == '*') {
        ret->setAll(true);
        advance();
    } else {
        QVariantMap newProps(props);
        newProps.insert("charset", "0-9a-zA-Z-_#$.");
        QScopedPointer<EntityNode> entity(minecraft_entity(parent, newProps));
        ret.reset(new Command::ScoreHolderNode(entity.get()));
    }
    return ret.take();
}

Command::ScoreboardSlotNode *Command::MinecraftParser::minecraft_scoreboardSlot(
    QObject *parent, const QVariantMap &props) {
    int     curPos = pos();
    QString slot   = this->oneOf({ "list", "sidebar.team",
                                   "sidebar", "belowName" });

    if (slot == "sidebar.team") {
        this->eat('.');
        QScopedPointer<Command::ColorNode> color(minecraft_color(this));
        slot += '.' + color->value();
    }
    return new Command::ScoreboardSlotNode(parent, curPos, slot);
}

Command::SwizzleNode *Command::MinecraftParser::minecraft_swizzle(
    QObject *parent, const QVariantMap &props) {
    QScopedPointer<Command::SwizzleNode> ret(
        new Command::SwizzleNode(parent, pos(), false, false, false));
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
    return ret.take();
}

Command::TeamNode *Command::MinecraftParser::minecraft_team(QObject *parent,
                                                            const QVariantMap &props)
{
    int     curPos  = pos();
    QString literal = this->getWithCharset(QStringLiteral("-\\w.:"));

    return new Command::TeamNode(parent, curPos, literal);
}

Command::TimeNode *Command::MinecraftParser::minecraft_time(QObject *parent,
                                                            const QVariantMap &props)
{
    int                       curPos = pos();
    auto                      unit   = Command::TimeNode::Unit::Tick;
    QScopedPointer<FloatNode> number(brigadier_float(this));

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
    return new Command::TimeNode(parent, curPos, pos() - curPos + 1,
                                 number->value(), unit);
}

Command::UuidNode *Command::MinecraftParser::minecraft_uuid(QObject *parent,
                                                            const QVariantMap &props)
{
    int     curPos = pos();
    QString uuid   = getWithRegex(QStringLiteral(
                                      R"([\da-fA-F]{8}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{12})"));

    return new Command::UuidNode(parent, curPos, uuid);
}

Command::Vec2Node *Command::MinecraftParser::minecraft_vec2(QObject *parent,
                                                            const QVariantMap &props)
{
    const QScopedPointer<Command::AxesNode> axes(parseAxes(parent,
                                                           AxisParseOption::
                                                           CanBeLocal));
    auto *ret = new Command::Vec2Node(parent, axes->pos(),
                                      axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}

Command::Vec3Node *Command::MinecraftParser::minecraft_vec3(QObject *parent,
                                                            const QVariantMap &props)
{
    const QScopedPointer<Command::AxesNode> axes(
        parseAxes(parent, AxisParseOption::CanBeLocal
                  | AxisParseOption::ParseY));
    auto *ret = new Command::Vec3Node(parent, axes->pos(),
                                      axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
    return ret;
}
