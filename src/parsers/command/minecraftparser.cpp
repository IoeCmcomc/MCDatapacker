#include "minecraftparser.h"

Command::MinecraftParser::MinecraftParser(QObject *parent,
                                          const QString &input)
    : Command::Parser(parent, input) {
    /*printMethods(); */
}

QString Command::MinecraftParser::oneOf(QStringList strArr) {
    QString    str;
    QStringRef curText = this->text().midRef(this->pos());

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

QVariantMap Command::MinecraftParser::curlyDict(std::function<QVariant(
                                                                  const QString &name)> func)
{
    this->eat('{');
    QVariantMap obj         = {};
    QString     nameCharset = "0-9a-zA-Z-_.:\\/+";
    while (this->curChar() != '}') {
        QString name = this->getWithCharset(nameCharset);
        this->skipWs();
        this->eat('=');
        this->skipWs();
        obj[name] = func(name);
        if (this->curChar() != '}') {
            this->skipWs();
            this->eat(',');
            this->skipWs();
        }
    }
    this->eat('}');
    return obj;
}

QVariantMap Command::MinecraftParser::squareDict(std::function<QVariant(
                                                                   const QString &name)> func)
{
    this->eat('[');
    QVariantMap obj         = {};
    QString     nameCharset = "a-z_";
    while (this->curChar() != ']') {
        QString name = this->getWithCharset(nameCharset);
        this->skipWs();
        this->eat('=');
        this->skipWs();
        obj[name] = func(name);
        if (this->curChar() != ']') {
            this->skipWs();
            this->eat(',');
            this->skipWs();
        }
    }
    this->eat(']');
    return obj;
}

QVariantMap Command::MinecraftParser::blockStates(QObject *parent) {
    return this->squareDict([this, parent](const QString &key) -> QVariant {
        auto *boolNode = this->brigadier_bool(parent);
        if (boolNode && boolNode->isVaild())
            return QVariant::fromValue(boolNode);
        else if (this->curChar().isNumber())
            return QVariant::fromValue(this->brigadier_integer(parent));
        else
            return this->getWithCharset("a-z_");
    });
}


Command::AxisNode *Command::MinecraftParser::parseAxis(QObject *parent,
                                                       AxisParseOptions options,
                                                       bool &isLocal) {
    auto   *axis           = new Command::AxisNode(parent);
    bool    canBeLocal     = options & AxisParseOption::CanBeLocal;
    bool    onlyInt        = options & AxisParseOption::OnlyInteger;
    QString mixCoordErrMsg =
        "Cannot mix world & local coordinates (everything must either use ^ or not).";
    auto axisVal = [ =, &isLocal](Command::AxisNode *axis) {
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
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<AxesNode> axes(parseAxes(
                                      parent,
                                      AxisParseOption::ParseY |
                                      AxisParseOption::OnlyInteger |
                                      AxisParseOption::CanBeLocal));
    auto *ret = new Command::BlockPosNode(parent, axes->pos(), axes->length());

    ret->setX(axes->x());
    ret->setY(axes->y());
    ret->setZ(axes->z());
    return ret;
}

Command::ColorNode *Command::MinecraftParser::minecraft_color(QObject *parent,
                                                              const QVariantMap &props)
{
    int     curPos  = pos();
    QString literal = oneOf({
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
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<Command::AxesNode> axes(parseAxes(
                                               parent,
                                               AxisParseOption::OnlyInteger |
                                               AxisParseOption::CanBeLocal));
    auto *ret = new Command::ColumnPosNode(parent, axes->pos(), axes->length());

    ret->setX(axes->x());
    ret->setZ(axes->z());
    return ret;
}



Command::ResourceLocationNode *Command::MinecraftParser::
minecraft_resourceLocation(QObject *parent, const QVariantMap &props) {
    int     curPos  = pos();
    QString nspace  = "minecraft";
    QString id      = "";
    QString charset = "0-9a-z-_\\/.";

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


Command::DimensionNode *Command::MinecraftParser::minecraft_dimension(
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::DimensionNode(parent,
                                      node->pos(),
                                      node->nspace(),
                                      node->id());
}

Command::FunctionNode *Command::MinecraftParser::minecraft_function(
    QObject *parent,
    const QVariantMap &props) {
    QScopedPointer<Command::ResourceLocationNode> node(
        minecraft_resourceLocation(parent));

    return new Command::FunctionNode(parent,
                                     node->pos(),
                                     node->nspace(),
                                     node->id());
}
