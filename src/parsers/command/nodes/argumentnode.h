#ifndef ARGUMENTNODE_H
#define ARGUMENTNODE_H

#include "parsenode.h"

#include <array>

#ifndef QLATIN1STRING_OPERATOR
QLatin1String constexpr operator ""_QL1(const char *literal, size_t size) {
    return QLatin1String(literal, size);
}
#define QLATIN1STRING_OPERATOR
#endif

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
            Gamemode,
            GameProfile,
            Heightmap,
            IntRange,
            ItemEnchantment,
            ItemPredicate,
            ItemSlot,
            ItemSlots,
            ItemStack,
            Item = ItemStack, // Obsolete
            LootModifier,
            LootPredicate,
            LootTable,
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
            ResourceKey,
            ResourceLocation,
            ResourceOrTag,
            ResourceOrTagKey,
            Rotation,
            ScoreHolder,
            ScoreboardSlot,
            Style,
            Swizzle,
            Team,
            TemplateMirror,
            TemplateRotation,
            Time,
            Uuid,
            Vec2,
            Vec3,
            Vanilla_end = 0xff,
            /*
             * Internal arguments are meant to use in custom syntax tree files
             * which are provided to the program by users.
             */
            InternalGreedyString,
            InternalRegexPattern,
        };

        void accept(NodeVisitor *visitor, VisitOrder) override;

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

    /*
     * Declaring staticSuggestions as QVector<QLatin1String> causes the program
     * to crash when exiting. The crash doesn't happen in debug mode.
     * Conclusion: QVector<QLatin1String> shouldn't be used with global variables.
     */
    template <class T>
    constexpr std::array<QLatin1String, 0> staticSuggestions;

    template<typename T, size_t N>
    QVector<QString> toStringVec(const std::array<T, N> &array) {
        QVector<QString> newVec{ (int)N };

        std::transform(array.cbegin(), array.cend(), newVec.begin(),
                       [](const auto &str) {
            return str;
        });
        return newVec;
    }
}

#endif /* ARGUMENTNODE_H */
