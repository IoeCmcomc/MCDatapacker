#ifndef ITEMSTACKNODE_H
#define ITEMSTACKNODE_H

#include "argumentnode.h"

namespace Command {
    class NbtCompoundNode;
    class ResourceLocationNode;
    class MapNode;

    class ItemStackNode : public ArgumentNode {
public:
        explicit ItemStackNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<ParseNode> components() const;
        void setComponents(QSharedPointer<ParseNode> components);

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

        QSharedPointer<ResourceLocationNode> resLoc() const;
        void setResLoc(QSharedPointer<ResourceLocationNode> newResLoc);

private:
        QSharedPointer<ResourceLocationNode> m_resLoc;
        QSharedPointer<ParseNode> m_components;
        QSharedPointer<NbtCompoundNode> m_nbt;
    };

    class ItemPredicateNode final : public ItemStackNode {
public:
        explicit ItemPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        bool isAll() const;
        void setAll(bool all);

private:
        bool m_all = false;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemStack)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemPredicate)
}

#endif /* ITEMSTACKNODE_H */
