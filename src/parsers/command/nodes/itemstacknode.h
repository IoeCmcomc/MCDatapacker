#ifndef ITEMSTACKNODE_H
#define ITEMSTACKNODE_H

#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class ItemStackNode : public ResourceLocationNode
    {
public:
        ItemStackNode(int pos, const QString &nspace, const QString &id);
        virtual QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            if (m_nbt)
                m_nbt->accept(visitor);
            visitor->visit(this);
        }

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

private:
        QSharedPointer<NbtCompoundNode> m_nbt = nullptr;
    };

    class ItemPredicateNode final : public ItemStackNode {
public:
        ItemPredicateNode(int pos, const QString &nspace,
                          const QString &id);
        ItemPredicateNode(ItemStackNode *other);
        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            if (nbt())
                nbt()->accept(visitor);
            visitor->visit(this);
        }
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ItemStackNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ItemPredicateNode>)

#endif /* ITEMSTACKNODE_H */
