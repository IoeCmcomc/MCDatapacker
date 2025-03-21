#include "itemstacknode.h"
#include "../visitors/nodevisitor.h"

#include "nbtnodes.h"
#include "resourcelocationnode.h"


namespace Command {
    ItemStackNode::ItemStackNode(int length)
        : ArgumentNode(ParserType::ItemStack, length) {
    }

    void ItemStackNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_resLoc)
            m_resLoc->accept(visitor, order);
        if (m_components)
            m_components->accept(visitor, order);
        if (m_nbt)
            m_nbt->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    QSharedPointer<ParseNode> ItemStackNode::components() const {
        return m_components;
    }

    void ItemStackNode::setComponents(QSharedPointer<ParseNode> components) {
        m_isValid   &= components->isValid();
        m_components = std::move(components);
    }

    QSharedPointer<NbtCompoundNode> ItemStackNode::nbt() const {
        return m_nbt;
    }

    void ItemStackNode::setNbt(QSharedPointer<NbtCompoundNode> nbt) {
        m_isValid &= nbt->isValid();
        m_nbt      = std::move(nbt);
    }

    QSharedPointer<ResourceLocationNode> ItemStackNode::resLoc() const {
        return m_resLoc;
    }

    void ItemStackNode::setResLoc(
        QSharedPointer<ResourceLocationNode> newResLoc) {
        m_resLoc  = std::move(newResLoc);
        m_isValid = m_resLoc->isValid();
    }

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
        if (resLoc())
            resLoc()->accept(visitor, order);
        if (nbt())
            nbt()->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool ItemPredicateNode::isAll() const {
        return m_all;
    }

    void ItemPredicateNode::setAll(bool all) {
        m_isValid = true;
        m_all     = all;
    }
}
