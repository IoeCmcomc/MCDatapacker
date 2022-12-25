#include "itemstacknode.h"
#include "../visitors/nodevisitor.h"

static bool _ = TypeRegister<Command::ItemStackNode>::init();

namespace Command {
    ItemStackNode::ItemStackNode(int length)
        : ResourceLocationNode(ParserType::ItemStack, length) {
    }

    bool ItemStackNode::isValid() const {
        return ResourceLocationNode::isValid() && m_nbt;
    }

    void ItemStackNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_nbt)
            m_nbt->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    QSharedPointer<NbtCompoundNode> ItemStackNode::nbt() const {
        return m_nbt;
    }

    void ItemStackNode::setNbt(QSharedPointer<NbtCompoundNode> nbt) {
        m_nbt = nbt;
    }

    static bool _2 = TypeRegister<ItemPredicateNode>::init();

    ItemPredicateNode::ItemPredicateNode(int length)
        : ItemStackNode(length) {
        m_parserType = ParserType::ItemPredicate;
    }

    void ItemPredicateNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (nbt())
            nbt()->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
}
