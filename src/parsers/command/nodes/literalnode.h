#ifndef LITERALNODE_H
#define LITERALNODE_H

#include "parsenode.h"

namespace Command {
    class LiteralNode : public ParseNode
    {
public:
        explicit LiteralNode(int pos, const QString &txt);

        QString toString() const override;
        QString text() const;
        void setText(const QString &text);

        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;

        bool isCommand() const;
        void setIsCommand(bool isCommand);

private:
        QString m_text;
        bool m_isCommand = false;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::LiteralNode>);

#endif /* LITERALNODE_H */
