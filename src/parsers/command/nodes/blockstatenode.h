#ifndef BLOCKSTATENODE_H
#define BLOCKSTATENODE_H

#include "argumentnode.h"


namespace Command {
    class MapNode;
    class NbtCompoundNode;
    class ResourceLocationNode;

    class BlockStateNode : public ArgumentNode {
public:
        explicit BlockStateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<MapNode> states() const;
        void setStates(QSharedPointer<MapNode> states);

        QSharedPointer<NbtCompoundNode> nbt() const;
        void setNbt(QSharedPointer<NbtCompoundNode> nbt);

        QSharedPointer<ResourceLocationNode> resLoc() const;
        void setResLoc(QSharedPointer<ResourceLocationNode> newResLoc);

private:
        QSharedPointer<ResourceLocationNode> m_resLoc;
        QSharedPointer<MapNode> m_states;
        QSharedPointer<NbtCompoundNode> m_nbt;
    };

    class BlockPredicateNode final : public BlockStateNode {
public:
        explicit BlockPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, BlockState)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, BlockPredicate)
}

#endif /* BLOCKSTATENODE_H */
