#ifndef MINECRAFTPARSER_H
#define MINECRAFTPARSER_H

#include "parser.h"
#include "nodes/similaraxesnodes.h"
#include "nodes/similarresourcelocationnodes.h"
#include "nodes/similarstringnodes.h"

namespace Command {
    class MinecraftParser : public Parser
    {
        Q_OBJECT
public:
        enum AxisParseOption {
            ParseY      = 1,
            OnlyInteger = 2,
            CanBeLocal  = 4,
        };
        Q_DECLARE_FLAGS(AxisParseOptions, AxisParseOption);

        explicit MinecraftParser(QObject *parent      = nullptr,
                                 const QString &input = "");
private:
        QString oneOf(QStringList strArr);
        QVariantMap curlyDict(std::function<QVariant(const QString &)> func);
        QVariantMap squareDict(std::function<QVariant(const QString &)> func);
        QVariantMap blockStates(QObject *parent);

        AxisNode *parseAxis(QObject *parent,
                            AxisParseOptions options,
                            bool &isLocal);
        AxesNode *parseAxes(QObject *parent, AxisParseOptions options);
        Q_INVOKABLE Command::BlockPosNode *minecraft_blockPos(QObject *parent,
                                                              const QVariantMap &props = {});

        Q_INVOKABLE Command::ColorNode *minecraft_color(QObject *parent,
                                                        const QVariantMap &props = {});

        Q_INVOKABLE Command::ColumnPosNode *minecraft_columnPos(QObject *parent,
                                                                const QVariantMap &props = {});

        Q_INVOKABLE Command::ResourceLocationNode *minecraft_resourceLocation(
            QObject *parent,
            const QVariantMap &props = {});
        Q_INVOKABLE Command::DimensionNode *minecraft_dimension(QObject *parent,
                                                                const QVariantMap &props = {});
        Q_INVOKABLE Command::FunctionNode *minecraft_function(QObject *parent,
                                                              const QVariantMap &props = {});
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Command::MinecraftParser::AxisParseOptions)

#endif /* MINECRAFTPARSER_H */
