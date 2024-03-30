#include "targetselectornode.h"
#include "../visitors/nodevisitor.h"

#include "mapnode.h"


namespace Command {
    TargetSelectorNode::TargetSelectorNode(int length)
        : ParseNode(Kind::Container, length) {
    }

    void TargetSelectorNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_args) {
            m_args->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    QSharedPointer<MapNode> TargetSelectorNode::args() const {
        return m_args;
    }

    void TargetSelectorNode::setArgs(QSharedPointer<MapNode> args) {
        m_isValid &= args->isValid();
        m_args     = std::move(args);
    }

    TargetSelectorNode::Variable TargetSelectorNode::variable()
    const {
        return m_variable;
    }

    void TargetSelectorNode::setVariable(const Variable &variable) {
        m_isValid  = true;
        m_variable = variable;
    }
}
