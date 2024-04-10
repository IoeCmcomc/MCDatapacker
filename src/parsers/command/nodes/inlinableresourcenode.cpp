#include "inlinableresourcenode.h"

#include "../visitors/nodevisitor.h"

namespace Command {
    void InlinableResourceNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_ptr)
            m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    NodePtr InlinableResourceNode::getNode() const {
        return m_ptr;
    }

    InlinableResourceNode::InlinableResourceNode(ParserType parserType,
                                                 int length,
                                                 const NodePtr &ptr)
        : ArgumentNode(parserType, length), m_ptr(ptr) {
    }


    LootModifierNode::LootModifierNode(int length)
        : InlinableResourceNode(ParserType::LootModifier, length) {
    }

    void LootModifierNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_ptr)
            m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }


    LootPredicateNode::LootPredicateNode(int length)
        : InlinableResourceNode(ParserType::LootPredicate, length) {
    }

    void LootPredicateNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_ptr)
            m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }


    LootTableNode::LootTableNode(int length)
        : InlinableResourceNode(ParserType::LootTable, length) {
    }

    void LootTableNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_ptr)
            m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
}
