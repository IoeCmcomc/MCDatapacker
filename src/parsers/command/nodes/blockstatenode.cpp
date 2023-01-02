#include "blockstatenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    BlockStateNode::BlockStateNode(int length)
        : ResourceLocationNode(ParserType::BlockState, length) {
    }

    void BlockStateNode::accept(NodeVisitor *visitor,
                                VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_states)
            m_states->accept(visitor, order);
        if (m_nbt)
            m_nbt->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool BlockStateNode::isValid() const {
        return ResourceLocationNode::isValid() && m_states && m_nbt;
    }


    QSharedPointer<MapNode> BlockStateNode::states() const {
        return m_states;
    }

    void BlockStateNode::setStates(QSharedPointer<MapNode> states) {
        m_states = states;
    }

    QSharedPointer<NbtCompoundNode> BlockStateNode::nbt()
    const {
        return m_nbt;
    }

    void BlockStateNode::setNbt(QSharedPointer<NbtCompoundNode> nbt) {
        m_nbt = nbt;
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
        if (states())
            states()->accept(visitor, order);
        if (nbt())
            nbt()->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
}
