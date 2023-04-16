#ifndef CODEPALETTE_H
#define CODEPALETTE_H

#include "parsers/command/nodes/nbtnodes.h"

#include <QTextCharFormat>

template <typename T>
class Builder {
public:
    inline T ok() const {
        return value;
    }

protected:
    T value;
};

#define BUILDER_METHOD(Name, Func, Arg)   \
        inline auto *Name(const Arg &v) { \
            value.Func(v);                \
            return this;                  \
        }                                 \

class CharFormatBuilder : public Builder<QTextCharFormat> {
public:
    BUILDER_METHOD(bg, setBackground, QBrush)
    BUILDER_METHOD(fg, setForeground, QBrush)
    BUILDER_METHOD(weight, setFontWeight, int)
    BUILDER_METHOD(italic, setFontItalic, bool)
    BUILDER_METHOD(strikeOut, setFontStrikeOut, bool)

    inline auto *bg(int r, int g, int b, int a = 255) {
        value.setBackground(QColor(r, g, b, a));
        return this;
    }

    inline auto * fg(int r, int g, int b, int a = 255) {
        value.setForeground(QColor(r, g, b, a));
        return this;
    }

    inline auto * bg(const char *hex) {
        value.setBackground(QColor(hex));
        return this;
    }

    inline auto * fg(const char *hex) {
        value.setForeground(QColor(hex));
        return this;
    }

    inline auto * bold(const bool isBold) {
        value.setFontWeight(isBold ? QFont::Bold : QFont::Normal);
        return this;
    }
};

class CodePalette {
    using NodeKind = Command::ParseNode::Kind;
    using ArgType  = Command::ArgumentNode::ParserType;
    using NbtType  = Command::NbtNode::TagType;

public:
    enum Role {
        Error      = (int)NodeKind::Error,
        CmdLine    = (int)NodeKind::Root,
        CmdLiteral = (int)NodeKind::Literal,
        UnknownArg = (int)NodeKind::Argument,
        Bool       = (int)ArgType::Bool,
        Double,
        Float,
        Integer,
        Long       = (int)ArgType::Long,
        String,
        Angle,
        BlockPos,
        BlockPredicate,
        BlockState,
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
        IntRange,
        ItemEnchantment,
        ItemPredicate,
        ItemSlot,
        ItemStack,
        Message,
        MobEffect,
        NbtPath,
        NbtTag = (int)ArgType::NbtTag,
        NbtByte,
        NbtDouble,
        NbtFloat,
        NbtInt,
        NbtLong,
        NbtShort,
        NbtByteArray,
        NbtIntArray,
        NbtLongArray,
        NbtString,
        NbtList,
        NbtCompoundTag = (int)ArgType::NbtCompoundTag,
        NbtCompound    = NbtCompoundTag,
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
        Swizzle,
        Team,
        TemplateMirror,
        TemplateRotation,
        Time,
        Uuid,
        Vec2,
        Vec3,
        Comment,
        Keyword,
        Number,
        Separator,
        Identifier,
        Macro,
        QuotedString,
        Bool_True,
        Bool_False,
        BlockState_States,
        NbtPath_Step,
        TargetSelector_Variable,
        TargetSelector_Arguments,
        Key,
        Operator       = Operation,
        CommandLiteral = Keyword,
    };

    using RoleToFormat = QMap<Role, QTextCharFormat>;

    CodePalette(std::initializer_list<QPair<Role, QTextCharFormat> > formats);

    inline const QTextCharFormat operator[](const Role role) const {
        return m_formats.value(role);
    }

    inline const QTextCharFormat operator[](const NodeKind kind) const {
        return m_formats.value(static_cast<Role>(kind));
    }

    inline const QTextCharFormat operator[](const ArgType type) const {
        return m_formats.value(static_cast<Role>(type));
    }

    inline const QTextCharFormat operator[](const NbtType type) const {
        return m_formats.value(
            static_cast<Role>(static_cast<int>(type) +
                              static_cast<int>(Role::NbtTag)));
    }

private:
    RoleToFormat m_formats;
};

const extern CodePalette defaultCodePalette;
const extern CodePalette defaultDarkCodePalette;

#endif // CODEPALETTE_H
