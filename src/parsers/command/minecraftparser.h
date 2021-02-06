#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/similarstringnodes.h"

namespace Command {
    class MinecraftParser : public Parser
    {
        Q_OBJECT
public:
        explicit MinecraftParser(QObject *parent      = nullptr,
                                 const QString &input = "");
private:
        QString oneOf(QStringList strArr);
        QVariantMap curlyDict(std::function<QVariant(const QString &)> func);
        QVariantMap squareDict(std::function<QVariant(const QString &)> func);
        QVariantMap blockStates(QObject *parent);
    };
}

#endif /* MINECRAFTPARSER_H */
