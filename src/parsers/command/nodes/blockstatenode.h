#ifndef BLOCKSTATENODE_H
#define BLOCKSTATENODE_H

#include "mapnode.h"
#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class BlockStateNode : public ResourceLocationNode
    {
        Q_OBJECT
public:
        BlockStateNode(QObject *parent, int pos, const QString &nspace,
                       const QString &id);
        virtual QString toString() const override;
        bool isVaild() const override;

        MapNode *states() const;
        void setStates(MapNode *states);

        NbtCompoundNode *nbt() const;
        void setNbt(NbtCompoundNode *nbt);

private:
        MapNode *m_states      = nullptr;
        NbtCompoundNode *m_nbt = nullptr;
    };

    class BlockPredicateNode final : public BlockStateNode {
        Q_OBJECT
public:
        BlockPredicateNode(QObject *parent, int pos, const QString &nspace,
                           const QString &id);
        BlockPredicateNode(BlockStateNode *other);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(Command::BlockStateNode*)

#endif /* BLOCKSTATENODE_H */
