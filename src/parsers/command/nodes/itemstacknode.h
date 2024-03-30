#ifndef ITEMSTACKNODE_H
#define ITEMSTACKNODE_H

#include "argumentnode.h"

namespace Command {
    class NbtCompoundNode;
    class ResourceLocationNode;

    class ItemStackNode : public ArgumentNode {
public:
        explicit ItemStackNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

        QSharedPointer<ResourceLocationNode> resLoc() const;
        void setResLoc(QSharedPointer<ResourceLocationNode> newResLoc);

private:
        QSharedPointer<ResourceLocationNode> m_resLoc;
        QSharedPointer<NbtCompoundNode> m_nbt;
    };

    class ItemPredicateNode final : public ItemStackNode {
public:
        explicit ItemPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemStack)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemPredicate)
}

#endif /* ITEMSTACKNODE_H */
