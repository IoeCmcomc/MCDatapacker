#include "entitynode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    EntityNode::EntityNode(int length)
        : ArgumentNode(ParserType::Entity, length) {
    }

    void EntityNode::accept(NodeVisitor *visitor, VisitOrder order) {
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

    bool EntityNode::playerOnly() const {
        return m_playerOnly;
    }

    void EntityNode::setPlayerOnly(bool playerOnly) {
        m_playerOnly = playerOnly;
    }

    NodePtr EntityNode::getNode() const {
        return m_ptr;
    }

    EntityNode::EntityNode(ParserType parserType, int length,
                           const NodePtr &ptr)
        : ArgumentNode(parserType, length), m_ptr(ptr) {
    }

    bool EntityNode::singleOnly() const {
        return m_singleOnly;
    }

    void EntityNode::setSingleOnly(bool singleOnly) {
        m_singleOnly = singleOnly;
    }

    GameProfileNode::GameProfileNode(int length)
        : EntityNode(ParserType::GameProfile, length, nullptr) {
    }

    void GameProfileNode::accept(NodeVisitor *visitor, VisitOrder order) {
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

    ScoreHolderNode::ScoreHolderNode(int length)
        : EntityNode(ParserType::ScoreHolder, length, nullptr) {
    }

    void ScoreHolderNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (!m_all) {
            if (m_ptr)
                m_ptr->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool ScoreHolderNode::isAll() const {
        return m_all;
    }

    void ScoreHolderNode::setAll(bool all) {
        m_isValid = true;
        m_all     = all;
    }

    InvertibleNode::InvertibleNode(bool inverted)
        : ParseNode(Kind::Container, 0), m_inverted(inverted) {
    }

    void InvertibleNode::accept(NodeVisitor *visitor,
                                VisitOrder order) {
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

    bool InvertibleNode::isInverted() const {
        return m_inverted;
    }

    void InvertibleNode::setInverted(bool inverted) {
        m_inverted = inverted;
    }

    NodePtr InvertibleNode::getNode()  const {
        return m_ptr;
    }
}
