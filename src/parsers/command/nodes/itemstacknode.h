#ifndef ITEMSTACKNODE_H
#define ITEMSTACKNODE_H

#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class ItemStackNode : public ResourceLocationNode
    {
        Q_OBJECT
public:
        ItemStackNode(QObject *parent, int pos,
                      const QString &nspace, const QString &id);
        virtual QString toString() const override;
        bool isVaild() const override;

        NbtCompoundNode *nbt() const;
        void setNbt(NbtCompoundNode *nbt);

private:
        NbtCompoundNode *m_nbt = nullptr;
    };

    class ItemPredicateNode final : public ItemStackNode {
        Q_OBJECT
public:
        ItemPredicateNode(QObject *parent, int pos, const QString &nspace,
                          const QString &id);
        ItemPredicateNode(ItemStackNode *other);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(Command::ItemStackNode*)
Q_DECLARE_METATYPE(Command::ItemPredicateNode*)

#endif /* ITEMSTACKNODE_H */
