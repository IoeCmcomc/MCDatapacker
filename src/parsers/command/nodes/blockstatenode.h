#ifndef BLOCKSTATENODE_H
#define BLOCKSTATENODE_H

#include "mapnode.h"
#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class BlockStateNode : public ResourceLocationNode
    {
public:
        BlockStateNode(int pos, const QString &nspace, const QString &id);
        virtual QString toString() const override;
        bool isVaild() const override;

        QSharedPointer<MapNode> states() const;
        void setStates(QSharedPointer<MapNode> states);

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

private:
        QSharedPointer<MapNode> m_states      = nullptr;
        QSharedPointer<NbtCompoundNode> m_nbt = nullptr;
    };

    class BlockPredicateNode final : public BlockStateNode {
public:
        BlockPredicateNode(int pos, const QString &nspace,
                           const QString &id);
        BlockPredicateNode(BlockStateNode *other);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::BlockStateNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::BlockPredicateNode>)

#endif /* BLOCKSTATENODE_H */
