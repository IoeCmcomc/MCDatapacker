#include "blockstatenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    BlockStateNode::BlockStateNode(int length)
        : ArgumentNode(ParserType::BlockState, length) {
    }

    void BlockStateNode::accept(NodeVisitor *visitor,
                                VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        Q_ASSERT(m_resLoc != nullptr);
        m_resLoc->accept(visitor, order);
        if (m_states)
            m_states->accept(visitor, order);
        if (m_nbt)
            m_nbt->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool BlockStateNode::isValid() const {
        return ArgumentNode::isValid() && m_states && m_nbt;
    }


    QSharedPointer<MapNode> BlockStateNode::states() const {
        return m_states;
    }

    void BlockStateNode::setStates(QSharedPointer<MapNode> states) {
        m_states = std::move(states);
    }

    QSharedPointer<NbtCompoundNode> BlockStateNode::nbt()  const {
        return m_nbt;
    }

    void BlockStateNode::setNbt(QSharedPointer<NbtCompoundNode> nbt) {
        m_nbt = std::move(nbt);
    }

    QSharedPointer<ResourceLocationNode> BlockStateNode::resLoc() const {
        return m_resLoc;
    }

    void BlockStateNode::setResLoc(
        QSharedPointer<ResourceLocationNode> newResLoc) {
        m_resLoc = std::move(newResLoc);
    }

    BlockPredicateNode::BlockPredicateNode(int length)
        : BlockStateNode(length) {
        m_parserType = ParserType::BlockPredicate;
    }

    void BlockPredicateNode::accept(NodeVisitor *visitor,
                                    VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        Q_ASSERT(resLoc() != nullptr);
        resLoc()->accept(visitor, order);
        if (states())
            states()->accept(visitor, order);
        if (nbt())
            nbt()->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
}
