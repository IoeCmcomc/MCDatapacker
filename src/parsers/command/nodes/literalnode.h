#ifndef LITERALNODE_H
#define LITERALNODE_H

#include "parsenode.h"

namespace Command {
    class LiteralNode : public ParseNode
    {
public:
        explicit LiteralNode(const QString &txt);

        void accept(NodeVisitor *visitor, VisitOrder) override;

        bool isCommand() const;
        void setIsCommand(bool isCommand);

private:
        bool m_isCommand = false;
    };

    DECLARE_TYPE_ENUM(ParseNode::Kind, Literal)
}

#endif /* LITERALNODE_H */
