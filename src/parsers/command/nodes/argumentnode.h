#ifndef ARGUMENTNODE_H
#define ARGUMENTNODE_H

#include "parsenode.h"

namespace Command {
    class ArgumentNode : public ParseNode {
public:
        enum class ParserType {
            Unknown = static_cast<int>(ParseNode::Kind::Argument),
            Bool,
            Double,
            Float,
            Integer,
            Literal, // Obsolete
            Long,    // Not used inside Minecraft
            String,
            Angle,
            BlockPos,
            BlockPredicate,
            BlockState,
            Block = BlockState, // Obsolete
            Color,
            ColumnPos,
            Component,
            Dimension,
            Entity,
            EntityAnchor,
            EntitySummon,
            FloatRange,
            Function,
            GameProfile,
            IntRange,
            ItemEnchantment,
            ItemPredicate,
            ItemSlot,
            ItemStack,
            Item = ItemStack, // Obsolete
            Message,
            MobEffect,
            NbtPath,
            NbtTag,
            NbtCompoundTag = NbtTag + 20,
            Objective,
            ObjectiveCriteria,
            Operation,
            Particle,
            Resource,
            ResourceKey, // Not documented online
            ResourceLocation,
            ResourceOrTag,
            ResourceOrTagKey, // Not documented online
            Rotation,
            ScoreHolder,
            ScoreboardSlot,
            Swizzle,
            Team,
            TemplateMirror,
            TemplateRotation,
            Time,
            Uuid,
            Vec2,
            Vec3,
        };

        virtual void accept(NodeVisitor *visitor, VisitOrder) override;

        ParserType parserType() const;

protected:
        ParserType m_parserType = ParserType::Unknown;

        explicit ArgumentNode(ParserType parserType)
            : ParseNode(Kind::Argument), m_parserType(parserType) {
        };
        explicit ArgumentNode(ParserType parserType, int length)
            : ParseNode(Kind::Argument, length), m_parserType(parserType) {
        };
        explicit ArgumentNode(ParserType parserType, const QString &text)
            : ParseNode(Kind::Argument, text), m_parserType(parserType) {
        };
    };

    template <class T>
    constexpr ArgumentNode::ParserType nodeTypeEnum<T,
                                                    ArgumentNode::ParserType> =
        ArgumentNode::ParserType::Unknown;
}

#endif /* ARGUMENTNODE_H */
