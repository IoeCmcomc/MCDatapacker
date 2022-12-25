#ifndef NODEVISITOR_H
#define NODEVISITOR_H

namespace Command {
    class ParseNode;
    class ErrorNode;
    class RootNode;
    class ArgumentNode;
    class BoolNode;
    class DoubleNode;
    class FloatNode;
    class IntegerNode;
    class StringNode;
    class LiteralNode;
    class AngleNode;
    class BlockStateNode;
    class ComponentNode;
    class EntityNode;
    class GameProfileNode;
    class ScoreHolderNode;
    class FloatRangeNode;
    class IntRangeNode;
    class ItemStackNode;
    class MapNode;
    class EntityArgumentValueNode;
    class NbtNode;
    class NbtByteArrayNode;
    class NbtByteNode;
    class NbtCompoundNode;
    class NbtDoubleNode;
    class NbtFloatNode;
    class NbtIntArrayNode;
    class NbtIntNode;
    class NbtListNode;
    class NbtLongArrayNode;
    class NbtLongNode;
    class NbtShortNode;
    class NbtStringNode;
    class NbtPathNode;
    class NbtPathStepNode;
    //class ResourceNode;
    //class ResourceOrTagNode;
    class ResourceLocationNode;
    class BlockPosNode;
    class BlockPredicateNode;
    class ColumnPosNode;
    class RotationNode;
    class Vec2Node;
    class Vec3Node;
    class DimensionNode;
    class EntitySummonNode;
    class FunctionNode;
    class ItemEnchantmentNode;
    class MobEffectNode;
    class ColorNode;
    class EntityAnchorNode;
    class ItemSlotNode;
    class MessageNode;
    class ObjectiveNode;
    class ObjectiveCriteriaNode;
    class OperationNode;
    class ScoreboardSlotNode;
    class TeamNode;
    class SwizzleNode;
    class TargetSelectorNode;
    class TimeNode;
    class UuidNode;
    class ParticleNode;
    class ParticleColorNode;
    class ItemPredicateNode;
    class KeyNode;

    enum VisitOrder: int { Preorder, Postorder, LetTheVisitorDecide };

    class NodeVisitor {
public:
        NodeVisitor(VisitOrder order) : m_order(order) {
        };

        virtual void startVisiting(ParseNode *node)       = 0;
        virtual void visit(ParseNode *node)               = 0;
        virtual void visit(ErrorNode *node)               = 0;
        virtual void visit(RootNode *node)                = 0;
        virtual void visit(ArgumentNode *node)            = 0;
        virtual void visit(BoolNode *node)                = 0;
        virtual void visit(DoubleNode *node)              = 0;
        virtual void visit(FloatNode *node)               = 0;
        virtual void visit(IntegerNode *node)             = 0;
        virtual void visit(StringNode *node)              = 0;
        virtual void visit(LiteralNode *node)             = 0;
        virtual void visit(AngleNode *node)               = 0;
        virtual void visit(BlockStateNode *node)          = 0;
        virtual void visit(ComponentNode *node)           = 0;
        virtual void visit(EntityNode *node)              = 0;
        virtual void visit(GameProfileNode *node)         = 0;
        virtual void visit(ScoreHolderNode *node)         = 0;
        virtual void visit(FloatRangeNode *node)          = 0;
        virtual void visit(IntRangeNode *node)            = 0;
        virtual void visit(ItemStackNode *node)           = 0;
        virtual void visit(MapNode *node)                 = 0;
        virtual void visit(EntityArgumentValueNode *node) = 0;
        virtual void visit(NbtNode *node)                 = 0;
        virtual void visit(NbtByteArrayNode *node)        = 0;
        virtual void visit(NbtByteNode *node)             = 0;
        virtual void visit(NbtCompoundNode *node)         = 0;
        virtual void visit(NbtDoubleNode *node)           = 0;
        virtual void visit(NbtFloatNode *node)            = 0;
        virtual void visit(NbtIntArrayNode *node)         = 0;
        virtual void visit(NbtIntNode *node)              = 0;
        virtual void visit(NbtListNode *node)             = 0;
        virtual void visit(NbtLongArrayNode *node)        = 0;
        virtual void visit(NbtLongNode *node)             = 0;
        virtual void visit(NbtShortNode *node)            = 0;
        virtual void visit(NbtStringNode *node)           = 0;
        virtual void visit(NbtPathNode *node)             = 0;
        virtual void visit(NbtPathStepNode *node)         = 0;
        virtual void visit(ResourceLocationNode *node)    = 0;
        virtual void visit(BlockPosNode *node)            = 0;
        virtual void visit(BlockPredicateNode *node)      = 0;
        virtual void visit(ColumnPosNode *node)           = 0;
        virtual void visit(RotationNode *node)            = 0;
        virtual void visit(Vec2Node *node)                = 0;
        virtual void visit(Vec3Node *node)                = 0;
        virtual void visit(DimensionNode *node)           = 0;
        virtual void visit(EntitySummonNode *node)        = 0;
        virtual void visit(FunctionNode *node)            = 0;
        virtual void visit(ItemEnchantmentNode *node)     = 0;
        virtual void visit(MobEffectNode *node)           = 0;
        virtual void visit(ColorNode *node)               = 0;
        virtual void visit(EntityAnchorNode *node)        = 0;
        virtual void visit(ItemSlotNode *node)            = 0;
        virtual void visit(MessageNode *node)             = 0;
        virtual void visit(ObjectiveNode *node)           = 0;
        virtual void visit(ObjectiveCriteriaNode *node)   = 0;
        virtual void visit(OperationNode *node)           = 0;
        virtual void visit(ScoreboardSlotNode *node)      = 0;
        virtual void visit(TeamNode *node)                = 0;
        virtual void visit(SwizzleNode *node)             = 0;
        virtual void visit(TargetSelectorNode *node)      = 0;
        virtual void visit(TimeNode *node)                = 0;
        virtual void visit(UuidNode *node)                = 0;
        virtual void visit(ParticleNode *node)            = 0;
        virtual void visit(ParticleColorNode *node)       = 0;
        virtual void visit(ItemPredicateNode *node)       = 0;
        virtual void visit(KeyNode *node)                 = 0;

protected:
        VisitOrder m_order = VisitOrder::Postorder;
    };
}

#endif /* NODEVISITOR_H */
