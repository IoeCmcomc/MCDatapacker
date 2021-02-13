#include "minecraftparser.h"

Command::MinecraftParser::MinecraftParser(QObject *parent,
                                          const QString &input)
    : Command::Parser(parent, input) {
    /*printMethods(); */
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

Command::MapNode *Command::MinecraftParser::parseMap(QObject *parent,
                                                     const QChar &beginChar,
                                                     const QChar &endChar,
                                                     const QChar &sepChar,
                                                     std::function<ParseNode*(const QString &)> func,
                                                     bool acceptQuotation) {
    auto *obj = new Command::MapNode(parent, pos());

    this->eat(beginChar);
    QString nameCharset = "0-9a-zA-Z-_.:\\/+";
    QString name;
    while (this->curChar() != endChar) {
        if (acceptQuotation) {
            try {
                name = getQuotedString();
            } catch (const Command::Parser::ParsingError &err) {
                /* No quotation have been found. Continue. */
            }
        }
        if (name.isNull())
            name = this->getWithCharset(nameCharset);
        this->skipWs();
        this->eat(sepChar);
        this->skipWs();
        (*obj)[name] = func(name);
        if (this->curChar() != endChar) {
            this->skipWs();
            this->eat(',');
            this->skipWs();
        }
    }
    this->eat(endChar);
    return obj;
}

Command::MapNode *Command::MinecraftParser::blockStates(QObject *parent) {
    return this->parseMap(parent, '{', '}', '=', [this, parent](
                              const QString &key) -> Command::ParseNode* {
        auto *boolNode = this->brigadier_bool(parent);
        if (boolNode && boolNode->isVaild())
            return boolNode;
        else if (this->curChar().isNumber())
            return this->brigadier_integer(parent);
        else
            return new Command::StringNode(parent, pos(),
                                           this->getWithCharset("a-z_"));

        delete boolNode;
    });
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
    bool  isLocal = false;
    auto *axes    = new Command::AxesNode(this, pos());

    axes->setX(parseAxis(axes, options, isLocal));

    if (options & AxisParseOption::ParseY) {
        this->eat(' ');
        axes->setY(parseAxis(axes, options, isLocal));
    }

    this->eat(' ');
    axes->setZ(parseAxis(axes, options, isLocal));
    return axes;
}

Command::BlockPosNode *Command::MinecraftParser::minecraft_blockPos(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<AxesNode> axes(parseAxes(parent,
                                                  AxisParseOption::ParseY |
                                                  AxisParseOption::OnlyInteger |
                                                  AxisParseOption::CanBeLocal));
    auto *ret = new Command::BlockPosNode(parent, axes->pos(), axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
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

Command::DimensionNode *Command::MinecraftParser::minecraft_dimension(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::DimensionNode(parent, node->pos(),
                                      node->nspace(), node->id());
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
    auto *num1 = this->brigadier_float(ret);
    if (!hasMax) {
        if (this->peek(2) == "..") {
            this->advance(2);
            if (curChar().isDigit() || curChar() == '.' || curChar() == '-') {
                ret->setMaxValue(this->brigadier_float(ret), true);
            }
            ret->setMinValue(num1, true);
        } else {
            ret->setExactValue(num1);
        }
    } else {
        ret->setMaxValue(num1, false);
    }
    ret->setLength(pos() - ret->pos() + 1);
    return ret;
}

Command::FunctionNode *Command::MinecraftParser::minecraft_function(
    QObject *parent, const QVariantMap &props) {
    const QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::FunctionNode(parent, node->pos(),
                                     node->nspace(), node->id());
}

Command::IntRangeNode *Command::MinecraftParser::minecraft_intRange(
    QObject *parent,
    const QVariantMap &props) {
    auto *ret    = new Command::IntRangeNode(parent, pos(), -1);
    bool  hasMax = false;

    if (this->peek(2) == "..") {
        hasMax = true;
        this->advance(2);
    }
    auto *num1 = this->brigadier_integer(ret);
    if (!hasMax) {
        if (this->peek(2) == "..") {
            this->advance(2);
            if (curChar().isDigit() || curChar() == '-') {
                ret->setMaxValue(brigadier_integer(ret), true);
            }
            ret->setMinValue(brigadier_integer(ret), true);
        } else {
            ret->setExactValue(num1);
        }
    } else {
        ret->setMaxValue(brigadier_integer(ret), false);
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

Command::MessageNode *Command::MinecraftParser::minecraft_message(
    QObject *parent,
    const QVariantMap &props) {
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

Command::ObjectiveNode *Command::MinecraftParser::minecraft_objective(
    QObject *parent,
    const QVariantMap &props) {
    int     curPos  = pos();
    QString objname = this->getWithCharset("0-9a-zA-Z-+_.");

    if (objname.length() > 16)
        this->error("objective " + objname + " must be less than 16 characters",
                    curPos, objname.length());
    return new Command::ObjectiveNode(parent, curPos, objname);
}

Command::ObjectiveCriteriaNode *Command::MinecraftParser::
minecraft_objectiveCriteria(QObject *parent, const QVariantMap &props) {
    int     curPos   = pos();
    QString criteria = this->getWithCharset("\\w-.:_");

    return new Command::ObjectiveCriteriaNode(parent, curPos, criteria);
}

Command::OperationNode *Command::MinecraftParser::minecraft_operation(
    QObject *parent, const QVariantMap &props) {
    const int curPos  = pos();
    QString   literal = oneOf(
        { "=", "<", ">", "><", "+=", "-=", "*=", "/=", "%=", });

    return new OperationNode(parent, curPos, literal);
}

Command::ResourceLocationNode *Command::MinecraftParser::
minecraft_resourceLocation(QObject *parent, const QVariantMap &props) {
    const int     curPos  = pos();
    QString       nspace  = "minecraft";
    QString       id      = "";
    const QString charset = "0-9a-z-_\\/.";

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
    QObject *parent,
    const QVariantMap &props) {
    const QScopedPointer<Command::AxesNode> axes(parseAxes(parent,
                                                           AxisParseOption::
                                                           NoOption));
    auto *ret = new Command::RotationNode(parent, axes->pos(),
                                          axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
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
    QObject *parent,
    const QVariantMap &props) {
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
    QString literal = this->getWithCharset("\\w-.:_");

    return new Command::TeamNode(parent, curPos, literal);
}

Command::TimeNode *Command::MinecraftParser::minecraft_time(QObject *parent,
                                                            const QVariantMap &props)
{
    int                                curPos = pos();
    auto                               unit   = Command::TimeNode::Unit::Tick;
    QScopedPointer<Command::FloatNode> number(brigadier_float(this));

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
    return new Command::TimeNode(parent, curPos, number->value(), unit);
}

Command::UuidNode *Command::MinecraftParser::minecraft_uuid(QObject *parent,
                                                            const QVariantMap &props)
{
    int     curPos = pos();
    QString uuid   = getWithRegex(
        R"([\da-fA-F]{8}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{12})");

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
    ret->setX(axes->y());
    ret->setZ(axes->z());
    return ret;
}
