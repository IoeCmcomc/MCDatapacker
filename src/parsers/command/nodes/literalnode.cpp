#include "literalnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    LiteralNode::LiteralNode(const QString &txt)
        : ParseNode(Kind::Literal, txt) {
        m_isValid = true;
    }

    void LiteralNode::accept(NodeVisitor *visitor, VisitOrder) {
        visitor->visit(this);
    }

    bool LiteralNode::isCommand() const {
        return m_isCommand;
    }

    void LiteralNode::setIsCommand(bool isCommand) {
        m_isCommand = isCommand;
    }
}
