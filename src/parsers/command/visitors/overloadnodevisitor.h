#ifndef OVERLOADNODEVISITOR_H
#define OVERLOADNODEVISITOR_H

#include "nodevisitor.h"

namespace Command {
    template <typename T>
    class OverloadNodeVisitor : public NodeVisitor
    {
public:
        OverloadNodeVisitor() = default;

        virtual void visit(ParseNode *node) {
            impl(node);
        }
        virtual void visit(RootNode *node) {
            impl(node);
        }
        virtual void visit(ArgumentNode *node) {
            impl(node);
        }
        virtual void visit(BoolNode *node) {
            impl(node);
        }
        virtual void visit(DoubleNode *node) {
            impl(node);
        }
        virtual void visit(FloatNode *node) {
            impl(node);
        }
        virtual void visit(IntegerNode *node) {
            impl(node);
        }
        virtual void visit(StringNode *node) {
            impl(node);
        }
        virtual void visit(LiteralNode *node) {
            impl(node);
        }
        virtual void visit(AngleNode *node) {
            impl(node);
        }
        virtual void visit(AxisNode *node) {
            impl(node);
        }
        virtual void visit(AxesNode *node) {
            impl(node);
        }
        virtual void visit(BlockStateNode *node) {
            impl(node);
        }
        virtual void visit(ComponentNode *node) {
            impl(node);
        }
        virtual void visit(EntityNode *node) {
            impl(node);
        }
        virtual void visit(GameProfileNode *node) {
            impl(node);
        }
        virtual void visit(ScoreHolderNode *node) {
            impl(node);
        }
        virtual void visit(FloatRangeNode *node) {
            impl(node);
        }
        virtual void visit(IntRangeNode *node) {
            impl(node);
        }
        virtual void visit(ItemStackNode *node) {
            impl(node);
        }
        virtual void visit(MapNode *node) {
            impl(node);
        }
        virtual void visit(MultiMapNode *node) {
            impl(node);
        }
        virtual void visit(EntityArgumentValueNode *node) {
            impl(node);
        }
        virtual void visit(NbtNode *node) {
            impl(node);
        }
        virtual void visit(NbtByteArrayNode *node) {
            impl(node);
        }
        virtual void visit(NbtByteNode *node) {
            impl(node);
        }
        virtual void visit(NbtCompoundNode *node) {
            impl(node);
        }
        virtual void visit(NbtDoubleNode *node) {
            impl(node);
        }
        virtual void visit(NbtFloatNode *node) {
            impl(node);
        }
        virtual void visit(NbtIntArrayNode *node) {
            impl(node);
        }
        virtual void visit(NbtIntNode *node) {
            impl(node);
        }
        virtual void visit(NbtListNode *node) {
            impl(node);
        }
        virtual void visit(NbtLongArrayNode *node) {
            impl(node);
        }
        virtual void visit(NbtLongNode *node) {
            impl(node);
        }
        virtual void visit(NbtShortNode *node) {
            impl(node);
        }
        virtual void visit(NbtStringNode *node) {
            impl(node);
        }
        virtual void visit(NbtPathNode *node) {
            impl(node);
        }
        virtual void visit(NbtPathStepNode *node) {
            impl(node);
        }
        virtual void visit(ResourceLocationNode *node) {
            impl(node);
        }
        virtual void visit(BlockPosNode *node) {
            impl(node);
        }
        virtual void visit(BlockPredicateNode *node) {
            impl(node);
        }
        virtual void visit(ColumnPosNode *node) {
            impl(node);
        }
        virtual void visit(RotationNode *node) {
            impl(node);
        }
        virtual void visit(Vec2Node *node) {
            impl(node);
        }
        virtual void visit(Vec3Node *node) {
            impl(node);
        }
        virtual void visit(DimensionNode *node) {
            impl(node);
        }
        virtual void visit(EntitySummonNode *node) {
            impl(node);
        }
        virtual void visit(FunctionNode *node) {
            impl(node);
        }
        virtual void visit(ItemEnchantmentNode *node) {
            impl(node);
        }
        virtual void visit(MobEffectNode *node) {
            impl(node);
        }
        virtual void visit(ColorNode *node) {
            impl(node);
        }
        virtual void visit(EntityAnchorNode *node) {
            impl(node);
        }
        virtual void visit(ItemSlotNode *node) {
            impl(node);
        }
        virtual void visit(MessageNode *node) {
            impl(node);
        }
        virtual void visit(ObjectiveNode *node) {
            impl(node);
        }
        virtual void visit(ObjectiveCriteriaNode *node) {
            impl(node);
        }
        virtual void visit(OperationNode *node) {
            impl(node);
        }
        virtual void visit(ScoreboardSlotNode *node) {
            impl(node);
        }
        virtual void visit(TeamNode *node) {
            impl(node);
        }
        virtual void visit(SwizzleNode *node) {
            impl(node);
        }
        virtual void visit(TargetSelectorNode *node) {
            impl(node);
        }
        virtual void visit(TimeNode *node) {
            impl(node);
        }
        virtual void visit(UuidNode *node) {
            impl(node);
        }
        virtual void visit(ParticleNode *node) {
            impl(node);
        }
        virtual void visit(ParticleColorNode *node) {
            impl(node);
        }
        virtual void visit(ItemPredicateNode *node) {
            impl(node);
        }
        virtual void visit(const MapKey &key) {
            impl(key);
        }

protected:
        T impl;
    };
}

#endif /* OVERLOADNODEVISITOR_H */
