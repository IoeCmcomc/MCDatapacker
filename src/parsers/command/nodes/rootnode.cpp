#include "rootnode.h"
#include "../visitors/nodevisitor.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::RootNode> >();

namespace Command {
    RootNode::RootNode(int length) : ParseNode(Kind::Root, length) {
    }

    void RootNode::accept(Command::NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        for (const auto &child: qAsConst(m_children)) {
            child->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool RootNode::isEmpty() {
        return m_children.empty();
    }

    int RootNode::size() {
        return m_children.size();
    }

    void RootNode::append(QSharedPointer<ParseNode> node) {
        m_children.push_back(node);
    }

    void RootNode::prepend(QSharedPointer<ParseNode> node) {
        m_children.push_front(node);
    }

    void RootNode::clear() {
        m_children.clear();
    }

    RootNode::Nodes RootNode::children()
    const {
        return m_children;
    }
}
