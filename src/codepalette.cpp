#include "codepalette.h"

CodePalette::CodePalette(std::initializer_list<QPair<Role,
                                                     QTextCharFormat> > formats)
{
    for (const auto& pair : formats) {
        if (!m_formats.contains(pair.first)) {
            m_formats.insert(pair.first, pair.second);
        } else {
            qWarning() << "Duplicate role ignored:" << pair.first;
        }
    }
}

using CP = CodePalette;
const CodePalette defaultCodePalette{
    { CP::CmdLiteral, CharFormatBuilder().fg("#0000AA")->bold(true)->ok() },
    { CP::Bool_True, CharFormatBuilder().fg(QColor(0, 190, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Bool_False, CharFormatBuilder().fg(QColor(190, 0, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Double, CharFormatBuilder().fg("#2F97C1")->ok() },
    { CP::Float, CharFormatBuilder().fg("#2F97B1")->ok() },
    { CP::Integer, CharFormatBuilder().fg("#2F97A1")->ok() },
    { CP::String, CharFormatBuilder().fg("#A31621")->ok() },
    { CP::Angle, CharFormatBuilder().fg("#7404cf")->ok() },
    { CP::BlockPos, CharFormatBuilder().fg("#8c54ba")->ok() },
    { CP::Vec3, CharFormatBuilder().fg("#8c54ba")->ok() },
    { CP::ColumnPos, CharFormatBuilder().fg("#8934cf")->ok() },
    { CP::Rotation, CharFormatBuilder().fg("#8934cf")->ok() },
    { CP::Vec2, CharFormatBuilder().fg("#8934cf")->ok() },
    { CP::BlockState, CharFormatBuilder().fg("#7d7b55")->ok() },
    { CP::BlockPredicate,
      CharFormatBuilder().fg("#7d7b55")->italic(true)->ok() },
    { CP::ItemStack, CharFormatBuilder().fg("#7d8b5a")->ok() },
    { CP::ItemPredicate,
      CharFormatBuilder().fg("#7d8b5a")->italic(true)->ok() },
    { CP::Component, CharFormatBuilder().bg("#aaffe8f2")->ok() },
    { CP::FloatRange, CharFormatBuilder().fg("#1e9cb0")->ok() },
    { CP::IntRange, CharFormatBuilder().fg("#1e9cb0")->ok() },
    { CP::NbtTag, CharFormatBuilder().fg("#226467")->ok() },
    { CP::NbtByteArray, CharFormatBuilder().bg("#aaf1edfa")->ok() },
    { CP::NbtCompound, CharFormatBuilder().bg("#aae8e9ff")->ok() },
    { CP::NbtList, CharFormatBuilder().bg("#aaf1edfa")->ok() },
    { CP::NbtIntArray, CharFormatBuilder().bg("#aaf1edfa")->ok() },
    { CP::ResourceLocation, CharFormatBuilder().fg("#45503B")->ok() },
    { CP::Resource, CharFormatBuilder().fg("#45503B")->ok() },
    { CP::ResourceOrTag,
      CharFormatBuilder().fg("#45503B")->italic(true)->ok() },
    { CP::ResourceKey, CharFormatBuilder().fg("#72912d")->ok() },
    { CP::ResourceOrTagKey,
      CharFormatBuilder().fg("#72912d")->italic(true)->ok() },
    { CP::Dimension, CharFormatBuilder().fg("#40677d")->ok() },
    { CP::EntitySummon, CharFormatBuilder().fg("#238500")->ok() },
    { CP::Function, CharFormatBuilder().fg("#13acba")->italic(true)->ok() },
    { CP::ItemEnchantment, CharFormatBuilder().fg("#7a077a")->ok() },
    { CP::MobEffect, CharFormatBuilder().fg("#7a6f82")->ok() },
    { CP::Color, CharFormatBuilder().fg("#565756")->ok() },
    { CP::EntityAnchor, CharFormatBuilder().fg("#8ca177")->ok() },
    { CP::ItemSlot, CharFormatBuilder().fg("#779fa1")->ok() },
    { CP::Objective, CharFormatBuilder().fg("#d41e7f")->italic(true)->ok() },
    { CP::ObjectiveCriteria, CharFormatBuilder().fg("#7a4c65")->ok() },
    { CP::Operation, CharFormatBuilder().fg("#13e853")->ok() },
    { CP::Particle, CharFormatBuilder().fg("#49B13B")->ok() },
    { CP::ScoreboardSlot, CharFormatBuilder().fg("#7d364b")->ok() },
    { CP::Team, CharFormatBuilder().fg("#826c4f")->ok() },
    { CP::Swizzle, CharFormatBuilder().fg("#de59ce")->ok() },
    { CP::Time, CharFormatBuilder().fg("#5C37A5")->ok() },
    { CP::Uuid, CharFormatBuilder().fg("#9c442f")->ok() },
    { CP::Comment, CharFormatBuilder().fg(QColor(62, 195, 0))->ok() },
    { CP::QuotedString, CharFormatBuilder().fg("#A31621")->ok() },
    { CP::CommandLiteral, CharFormatBuilder().fg(Qt::blue)->bold(true)->ok() },
    { CP::QuotedString, CharFormatBuilder().fg("#a33016")->ok() },
    { CP::BlockState_States, CharFormatBuilder().bg("#aafaf0f5")->ok() },
    { CP::NbtPath_Step, CharFormatBuilder().bg("#aafff4e8")->ok() },
    { CP::TargetSelector_Variable, CharFormatBuilder().fg("#1e9c11")->weight(
          QFont::DemiBold)->ok() },
    { CP::TargetSelector_Arguments, CharFormatBuilder().bg("#aafaf0f5")->ok() },
    { CP::Key, CharFormatBuilder().fg("#B89C3D")->ok() },
};
