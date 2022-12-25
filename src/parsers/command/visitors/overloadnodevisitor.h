#ifndef OVERLOADNODEVISITOR_H
#define OVERLOADNODEVISITOR_H

#include "nodevisitor.h"

#include "../nodes/rootnode.h"
#include "../nodes/stringnode.h"
#include "../nodes/axesnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/componentnode.h"
#include "../nodes/entitynode.h"
#include "../nodes/floatrangenode.h"
#include "../nodes/intrangenode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/swizzlenode.h"
#include "../nodes/timenode.h"

namespace Command {
    template <typename T>
    class OverloadNodeVisitor : public NodeVisitor {
public:
        explicit OverloadNodeVisitor(VisitOrder order) : NodeVisitor(order) {
        };

        virtual void visit(ParseNode *node) override {
            impl(node);
        }
        virtual void visit(ErrorNode *node) override {
            impl(node);
        }
        virtual void visit(RootNode *node) override {
            impl(node);
        }
        virtual void visit(ArgumentNode *node) override {
            impl(node);
        }
        virtual void visit(BoolNode *node) override {
            impl(node);
        }
        virtual void visit(DoubleNode *node) override {
            impl(node);
        }
        virtual void visit(FloatNode *node) override {
            impl(node);
        }
        virtual void visit(IntegerNode *node) override {
            impl(node);
        }
        virtual void visit(StringNode *node) override {
            impl(node);
        }
        virtual void visit(LiteralNode *node) override {
            impl(node);
        }
        virtual void visit(AngleNode *node) override {
            impl(node);
        }
        virtual void visit(BlockStateNode *node) override {
            impl(node);
        }
        virtual void visit(ComponentNode *node) override {
            impl(node);
        }
        virtual void visit(EntityNode *node) override {
            impl(node);
        }
        virtual void visit(GameProfileNode *node) override {
            impl(node);
        }
        virtual void visit(ScoreHolderNode *node) override {
            impl(node);
        }
        virtual void visit(FloatRangeNode *node) override {
            impl(node);
        }
        virtual void visit(IntRangeNode *node) override {
            impl(node);
        }
        virtual void visit(ItemStackNode *node) override {
            impl(node);
        }
        virtual void visit(MapNode *node) override {
            impl(node);
        }
        virtual void visit(EntityArgumentValueNode *node) override {
            impl(node);
        }
        virtual void visit(NbtNode *node) override {
            impl(node);
        }
        virtual void visit(NbtByteArrayNode *node) override {
            impl(node);
        }
        virtual void visit(NbtByteNode *node) override {
            impl(node);
        }
        virtual void visit(NbtCompoundNode *node) override {
            impl(node);
        }
        virtual void visit(NbtDoubleNode *node) override {
            impl(node);
        }
        virtual void visit(NbtFloatNode *node) override {
            impl(node);
        }
        virtual void visit(NbtIntArrayNode *node) override {
            impl(node);
        }
        virtual void visit(NbtIntNode *node) override {
            impl(node);
        }
        virtual void visit(NbtListNode *node) override {
            impl(node);
        }
        virtual void visit(NbtLongArrayNode *node) override {
            impl(node);
        }
        virtual void visit(NbtLongNode *node) override {
            impl(node);
        }
        virtual void visit(NbtShortNode *node) override {
            impl(node);
        }
        virtual void visit(NbtStringNode *node) override {
            impl(node);
        }
        virtual void visit(NbtPathNode *node) override {
            impl(node);
        }
        virtual void visit(NbtPathStepNode *node) override {
            impl(node);
        }
        virtual void visit(ResourceLocationNode *node) override {
            impl(node);
        }
        virtual void visit(BlockPosNode *node) override {
            impl(node);
        }
        virtual void visit(BlockPredicateNode *node) override {
            impl(node);
        }
        virtual void visit(ColumnPosNode *node) override {
            impl(node);
        }
        virtual void visit(RotationNode *node) override {
            impl(node);
        }
        virtual void visit(Vec2Node *node) override {
            impl(node);
        }
        virtual void visit(Vec3Node *node) override {
            impl(node);
        }
        virtual void visit(DimensionNode *node) override {
            impl(node);
        }
        virtual void visit(EntitySummonNode *node) override {
            impl(node);
        }
        virtual void visit(FunctionNode *node) override {
            impl(node);
        }
        virtual void visit(ItemEnchantmentNode *node) override {
            impl(node);
        }
        virtual void visit(MobEffectNode *node) override {
            impl(node);
        }
        virtual void visit(ColorNode *node) override {
            impl(node);
        }
        virtual void visit(EntityAnchorNode *node) override {
            impl(node);
        }
        virtual void visit(ItemSlotNode *node) override {
            impl(node);
        }
        virtual void visit(MessageNode *node) override {
            impl(node);
        }
        virtual void visit(ObjectiveNode *node) override {
            impl(node);
        }
        virtual void visit(ObjectiveCriteriaNode *node) override {
            impl(node);
        }
        virtual void visit(OperationNode *node) override {
            impl(node);
        }
        virtual void visit(ScoreboardSlotNode *node) override {
            impl(node);
        }
        virtual void visit(TeamNode *node) override {
            impl(node);
        }
        virtual void visit(SwizzleNode *node) override {
            impl(node);
        }
        virtual void visit(TargetSelectorNode *node) override {
            impl(node);
        }
        virtual void visit(TimeNode *node) override {
            impl(node);
        }
        virtual void visit(UuidNode *node) override {
            impl(node);
        }
        virtual void visit(ParticleNode *node) override {
            impl(node);
        }
        virtual void visit(ParticleColorNode *node) override {
            impl(node);
        }
        virtual void visit(ItemPredicateNode *node) override {
            impl(node);
        }
        virtual void visit(KeyNode *node) override {
            impl(node);
        }

protected:
        T impl;
    };

    class TestImpl {
        void operator()(ParseNode *) {
        }
    };

    class Test : public OverloadNodeVisitor<TestImpl> {
    };
}

#endif /* OVERLOADNODEVISITOR_H */
