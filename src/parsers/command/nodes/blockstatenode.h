#ifndef BLOCKSTATENODE_H
#define BLOCKSTATENODE_H

#include "mapnode.h"
#include "nbtnodes.h"
#include "resourcelocationnode.h"

namespace Command {
    class BlockStateNode : public ResourceLocationNode  {
public:
        explicit BlockStateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
        bool isValid() const override;

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
        explicit BlockPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, BlockState)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, BlockPredicate)
}

#endif /* BLOCKSTATENODE_H */
