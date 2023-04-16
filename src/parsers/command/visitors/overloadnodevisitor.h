#ifndef OVERLOADNODEVISITOR_H
#define OVERLOADNODEVISITOR_H

#include "nodevisitor.h"

#include "../nodes/rootnode.h"
#include "../nodes/literalnode.h"
#include "../nodes/stringnode.h"
#include "../nodes/axesnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/componentnode.h"
#include "../nodes/gamemodenode.h"
#include "../nodes/entitynode.h"
#include "../nodes/floatrangenode.h"
#include "../nodes/intrangenode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/swizzlenode.h"
#include "../nodes/timenode.h"
#include "../nodes/targetselectornode.h"

namespace Command {
    template <class T, class U>
    static constexpr bool is_base_of =
        !std::is_same_v<T, U> && std::is_base_of_v<T, U>;

    class OverloadNodeVisitor : public NodeVisitor {
public:
        explicit OverloadNodeVisitor(VisitOrder order) : NodeVisitor(order) {
        };

        virtual void visit([[maybe_unused]] ParseNode *node) override {
        }
        virtual void visit(ErrorNode *node) override {
            _visit(node);
            //visit(static_cast<ParseNode*>(node));
        }
        virtual void visit(RootNode *node) override {
            _visit(node);
        }
        virtual void visit(ArgumentNode *node) override {
            _visit(node);
        }
        virtual void visit(BoolNode *node) override {
            _visit(node);
        }
        virtual void visit(DoubleNode *node) override {
            _visit(node);
        }
        virtual void visit(FloatNode *node) override {
            _visit(node);
        }
        virtual void visit(IntegerNode *node) override {
            _visit(node);
        }
        virtual void visit(StringNode *node) override {
            _visit(node);
        }
        virtual void visit(LiteralNode *node) override {
            _visit(node);
        }
        virtual void visit(AngleNode *node) override {
            _visit(node);
        }
        virtual void visit(BlockStateNode *node) override {
            _visit(node);
        }
        virtual void visit(ComponentNode *node) override {
            _visit(node);
        }
        virtual void visit(EntityNode *node) override {
            _visit(node);
        }
        virtual void visit(GameProfileNode *node) override {
            _visit(node);
        }
        virtual void visit(ScoreHolderNode *node) override {
            _visit(node);
        }
        virtual void visit(FloatRangeNode *node) override {
            _visit(node);
        }
        virtual void visit(IntRangeNode *node) override {
            _visit(node);
        }
        virtual void visit(ItemStackNode *node) override {
            _visit(node);
        }
        virtual void visit(MapNode *node) override {
            _visit(node);
        }
        virtual void visit(EntityArgumentValueNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtByteArrayNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtByteNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtCompoundNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtDoubleNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtFloatNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtIntArrayNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtIntNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtListNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtLongArrayNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtLongNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtShortNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtStringNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtPathNode *node) override {
            _visit(node);
        }
        virtual void visit(NbtPathStepNode *node) override {
            _visit(node);
        }
        virtual void visit(ResourceLocationNode *node) override {
            _visit(node);
        }
        virtual void visit(ResourceNode *node) override {
            _visit(node);
        }
        virtual void visit(ResourceKeyNode *node) override {
            _visit(node);
        }
        virtual void visit(ResourceOrTagNode *node) override {
            _visit(node);
        }
        virtual void visit(ResourceOrTagKeyNode *node) override {
            _visit(node);
        }
        virtual void visit(BlockPosNode *node) override {
            _visit(node);
        }
        virtual void visit(BlockPredicateNode *node) override {
            _visit(node);
        }
        virtual void visit(ColumnPosNode *node) override {
            _visit(node);
        }
        virtual void visit(RotationNode *node) override {
            _visit(node);
        }
        virtual void visit(Vec2Node *node) override {
            _visit(node);
        }
        virtual void visit(Vec3Node *node) override {
            _visit(node);
        }
        virtual void visit(DimensionNode *node) override {
            _visit(node);
        }
        virtual void visit(EntitySummonNode *node) override {
            _visit(node);
        }
        virtual void visit(FunctionNode *node) override {
            _visit(node);
        }
        virtual void visit(ItemEnchantmentNode *node) override {
            _visit(node);
        }
        virtual void visit(MobEffectNode *node) override {
            _visit(node);
        }
        virtual void visit(ColorNode *node) override {
            _visit(node);
        }
        virtual void visit(EntityAnchorNode *node) override {
            _visit(node);
        }
        virtual void visit(ItemSlotNode *node) override {
            _visit(node);
        }
        virtual void visit(MessageNode *node) override {
            _visit(node);
        }
        virtual void visit(ObjectiveNode *node) override {
            _visit(node);
        }
        virtual void visit(ObjectiveCriteriaNode *node) override {
            _visit(node);
        }
        virtual void visit(OperationNode *node) override {
            _visit(node);
        }
        virtual void visit(ScoreboardSlotNode *node) override {
            _visit(node);
        }
        virtual void visit(TeamNode *node) override {
            _visit(node);
        }
        virtual void visit(TemplateMirrorNode *node) override {
            _visit(node);
        }
        virtual void visit(TemplateRotationNode *node) override {
            _visit(node);
        }
        virtual void visit(SwizzleNode *node) override {
            _visit(node);
        }
        virtual void visit(TargetSelectorNode *node) override {
            _visit(node);
        }
        virtual void visit(TimeNode *node) override {
            _visit(node);
        }
        virtual void visit(UuidNode *node) override {
            _visit(node);
        }
        virtual void visit(ParticleNode *node) override {
            _visit(node);
        }
        virtual void visit(ParticleColorNode *node) override {
            _visit(node);
        }
        virtual void visit(ItemPredicateNode *node) override {
            _visit(node);
        }
        virtual void visit(KeyNode *node) override {
            _visit(node);
        }

        virtual void visit(TwoAxesNode *node) override {
            _visit(node);
        }

        virtual void visit(XyzNode *node) override {
            _visit(node);
        }

        virtual void visit(GamemodeNode *node) override {
            _visit(node);
        }

private:
        template <class U>
        void _visit(U *node) {
            if constexpr (is_base_of<BlockStateNode, U>) {
                visit(static_cast<BlockStateNode *>(node));
            } else if constexpr (is_base_of<EntityNode, U>) {
                visit(static_cast<EntityNode *>(node));
            } else if constexpr (is_base_of<ItemStackNode, U>) {
                visit(static_cast<ItemStackNode *>(node));
            } else if constexpr (is_base_of<NbtNode, U>) {
                visit(static_cast<NbtNode *>(node));
            } else if constexpr (is_base_of<ResourceLocationNode, U>) {
                visit(static_cast<ResourceLocationNode *>(node));
            } else if constexpr (is_base_of<TwoAxesNode, U>) {
                visit(static_cast<TwoAxesNode *>(node));
            } else if constexpr (is_base_of<XyzNode, U>) {
                visit(static_cast<XyzNode *>(node));
            } else if constexpr (is_base_of<ArgumentNode, U>) {
                visit(static_cast<ArgumentNode *>(node));
            } else if constexpr (is_base_of<ParseNode, U>) {
                visit(static_cast<ParseNode *>(node));
            }
        }
    };
}

#endif /* OVERLOADNODEVISITOR_H */
