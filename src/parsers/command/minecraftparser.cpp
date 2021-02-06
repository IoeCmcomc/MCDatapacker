#include "minecraftparser.h"

Command::MinecraftParser::MinecraftParser(QObject *parent,
                                          const QString &input)
    : Command::Parser(parent, input) {
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
/*"\\b[a-z0-9-_]+:[a-z0-9-_/.]+\\b" */

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

