#ifndef ITEMSTACKNODE_H
#define ITEMSTACKNODE_H

#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class ItemStackNode : public ResourceLocationNode
    {
public:
        explicit ItemStackNode(int length);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

private:
        QSharedPointer<NbtCompoundNode> m_nbt = nullptr;
    };

    class ItemPredicateNode final : public ItemStackNode {
public:
        explicit ItemPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemStack)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ItemPredicate)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ItemStackNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ItemPredicateNode>)

#endif /* ITEMSTACKNODE_H */
