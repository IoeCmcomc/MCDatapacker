#include "codepalette.h"

#include <QDebug>

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
    { CP::CmdLiteral, CharFmtBuilder().fg("#0000AA")->bold(true)->ok() },
    { CP::Bool_True, CharFmtBuilder().fg(QColor(0, 190, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Bool_False, CharFmtBuilder().fg(QColor(190, 0, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Double, CharFmtBuilder().fg("#2F97C1")->ok() },
    { CP::Float, CharFmtBuilder().fg("#2F97C1")->ok() },
    { CP::Integer, CharFmtBuilder().fg("#2F97C1")->ok() },
    { CP::Long, CharFmtBuilder().fg("#2F97C1")->ok() },
    { CP::String, CharFmtBuilder().fg("#A31621")->ok() },
    { CP::Angle, CharFmtBuilder().fg("#7404cf")->ok() },
    { CP::BlockPos, CharFmtBuilder().fg("#8c54ba")->ok() },
    { CP::BlockPredicate, CharFmtBuilder().fg("#7d7b55")->italic(true)->ok() },
    { CP::BlockState, CharFmtBuilder().fg("#7d7b55")->ok() },
    { CP::Color, CharFmtBuilder().fg("#565756")->ok() },
    { CP::ColumnPos, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::Component, CharFmtBuilder().bg("#aaffe8f2")->ok() },
    { CP::Dimension, CharFmtBuilder().fg("#40677d")->ok() },
    { CP::EntityAnchor, CharFmtBuilder().fg("#8ca177")->ok() },
    { CP::EntitySummon, CharFmtBuilder().fg("#238500")->ok() },
    { CP::FloatRange, CharFmtBuilder().fg("#1e9cb0")->ok() },
    { CP::Function, CharFmtBuilder().fg("#13acba")->italic(true)->ok() },
    { CP::Heightmap, CharFmtBuilder().fg("#6B6B6B")->ok() },
    { CP::IntRange, CharFmtBuilder().fg("#1e9cb0")->ok() },
    { CP::ItemEnchantment, CharFmtBuilder().fg("#7a077a")->ok() },
    { CP::ItemPredicate, CharFmtBuilder().fg("#7d8b5a")->italic(true)->ok() },
    { CP::ItemSlot, CharFmtBuilder().fg("#779fa1")->ok() },
    { CP::ItemSlots, CharFmtBuilder().fg("#779fa1")->ok() },
    { CP::ItemStack, CharFmtBuilder().fg("#7d8b5a")->ok() },
    { CP::MobEffect, CharFmtBuilder().fg("#7a6f82")->ok() },
    { CP::NbtByteArray, CharFmtBuilder().bg("#aaf1edfa")->ok() },
    { CP::NbtCompound, CharFmtBuilder().bg("#aae8e9ff")->ok() },
    { CP::NbtIntArray, CharFmtBuilder().bg("#aaf1edfa")->ok() },
    { CP::NbtList, CharFmtBuilder().bg("#aaf1edfa")->ok() },
    { CP::NbtTag, CharFmtBuilder().fg("#226467")->ok() },
    { CP::Objective, CharFmtBuilder().fg("#d41e7f")->italic(true)->ok() },
    { CP::ObjectiveCriteria, CharFmtBuilder().fg("#7a4c65")->ok() },
    { CP::Operation, CharFmtBuilder().fg("#13e853")->ok() },
    { CP::Particle, CharFmtBuilder().fg("#49B13B")->ok() },
    { CP::Resource, CharFmtBuilder().fg("#45503B")->ok() },
    { CP::ResourceKey, CharFmtBuilder().fg("#72912d")->ok() },
    { CP::ResourceLocation, CharFmtBuilder().fg("#45503B")->ok() },
    { CP::ResourceOrTag, CharFmtBuilder().fg("#45503B")->italic(true)->ok() },
    { CP::ResourceOrTagKey,
      CharFmtBuilder().fg("#72912d")->italic(true)->ok() },
    { CP::Rotation, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::ScoreboardSlot, CharFmtBuilder().fg("#7d364b")->ok() },
    { CP::Style, CharFmtBuilder().bg("#aaffe8f2")->ok() },
    { CP::Swizzle, CharFmtBuilder().fg("#de59ce")->ok() },
    { CP::Team, CharFmtBuilder().fg("#826c4f")->ok() },
    { CP::TemplateMirror, CharFmtBuilder().fg("#666930")->ok() },
    { CP::TemplateRotation, CharFmtBuilder().fg("#647116")->ok() },
    { CP::Time, CharFmtBuilder().fg("#5C37A5")->ok() },
    { CP::Uuid, CharFmtBuilder().fg("#9c442f")->ok() },
    { CP::Vec2, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::Vec3, CharFmtBuilder().fg("#8c54ba")->ok() },
    { CP::Comment, CharFmtBuilder().fg(QColor(62, 195, 0))->ok() },
    { CP::Number, CharFmtBuilder().fg("#2F97C1")->ok() },
    { CP::Directive, CharFmtBuilder().fg("#985541")->bold(true)->ok() },
    { CP::Script, CharFmtBuilder().bg("#aaEBEBEB")->ok() },
    { CP::QuotedString, CharFmtBuilder().fg("#A31621")->ok() },
    { CP::CommandLiteral, CharFmtBuilder().fg(Qt::blue)->bold(true)->ok() },
    { CP::BlockState_States, CharFmtBuilder().bg("#aafaf0f5")->ok() },
    { CP::NbtPath_Step, CharFmtBuilder().bg("#aafff4e8")->ok() },
    { CP::TargetSelector_Variable, CharFmtBuilder().fg("#1e9c11")->weight(
          QFont::DemiBold)->ok() },
    { CP::TargetSelector_Arguments, CharFmtBuilder().bg("#aafaf0f5")->ok() },
    { CP::ItemStack_Components, CharFmtBuilder().bg("#aafae8f2")->ok() },
    { CP::Key, CharFmtBuilder().fg("#B89C3D")->ok() },
    { CP::Gamemode, CharFmtBuilder().fg("#152769")->bold(true)->ok() },
    { CP::RegexPattern, CharFmtBuilder().fg("#444444")->ok() },
    { CP::GreedyString, CharFmtBuilder().fg("#A31621")->ok() },
};

const CodePalette defaultDarkCodePalette{
    { CP::CmdLiteral, CharFmtBuilder().fg("#9595FF")->bold(true)->ok() },
    { CP::Bool_True, CharFmtBuilder().fg(QColor(0, 190, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Bool_False, CharFmtBuilder().fg(QColor(190, 0, 0))
      ->weight(QFont::DemiBold)->ok() },
    { CP::Double, CharFmtBuilder().fg("#A2D4E8")->ok() },
    { CP::Float, CharFmtBuilder().fg("#A2D4E8")->ok() },
    { CP::Integer, CharFmtBuilder().fg("#A2D4E8")->ok() },
    { CP::Long, CharFmtBuilder().fg("#A2D4E8")->ok() },
    { CP::String, CharFmtBuilder().fg("#B72E3A")->ok() },
    { CP::Angle, CharFmtBuilder().fg("#7404cf")->ok() },
    { CP::BlockPos, CharFmtBuilder().fg("#8c54ba")->ok() },
    { CP::BlockPredicate, CharFmtBuilder().fg("#7d7b55")->italic(true)->ok() },
    { CP::BlockState, CharFmtBuilder().fg("#7d7b55")->ok() },
    { CP::Color, CharFmtBuilder().fg("#565756")->ok() },
    { CP::ColumnPos, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::Component, CharFmtBuilder().bg("#aa45001E")->ok() },
    { CP::Dimension, CharFmtBuilder().fg("#588CA9")->ok() },
    { CP::EntityAnchor, CharFmtBuilder().fg("#8ca177")->ok() },
    { CP::EntitySummon, CharFmtBuilder().fg("#238500")->ok() },
    { CP::FloatRange, CharFmtBuilder().fg("#1e9cb0")->ok() },
    { CP::Function, CharFmtBuilder().fg("#13acba")->italic(true)->ok() },
    { CP::Heightmap, CharFmtBuilder().fg("#969696")->ok() },
    { CP::IntRange, CharFmtBuilder().fg("#1e9cb0")->ok() },
    { CP::ItemEnchantment, CharFmtBuilder().fg("#7a077a")->ok() },
    { CP::ItemPredicate, CharFmtBuilder().fg("#7d8b5a")->italic(true)->ok() },
    { CP::ItemSlot, CharFmtBuilder().fg("#779fa1")->ok() },
    { CP::ItemSlots, CharFmtBuilder().fg("#779fa1")->ok() },
    { CP::ItemStack, CharFmtBuilder().fg("#7d8b5a")->ok() },
    { CP::MobEffect, CharFmtBuilder().fg("#7a6f82")->ok() },
    { CP::NbtByteArray, CharFmtBuilder().bg("#aa281651")->ok() },
    { CP::NbtCompound, CharFmtBuilder().bg("#aa000568")->ok() },
    { CP::NbtIntArray, CharFmtBuilder().bg("#aa281651")->ok() },
    { CP::NbtList, CharFmtBuilder().bg("#aa281651")->ok() },
    { CP::NbtTag, CharFmtBuilder().fg("#3DB1B7")->ok() },
    { CP::Objective, CharFmtBuilder().fg("#d41e7f")->italic(true)->ok() },
    { CP::ObjectiveCriteria, CharFmtBuilder().fg("#7a4c65")->ok() },
    { CP::Operation, CharFmtBuilder().fg("#13e853")->ok() },
    { CP::Particle, CharFmtBuilder().fg("#49B13B")->ok() },
    { CP::Resource, CharFmtBuilder().fg("#94A585")->ok() },
    { CP::ResourceKey, CharFmtBuilder().fg("#8CB337")->ok() },
    { CP::ResourceLocation, CharFmtBuilder().fg("#94A585")->ok() },
    { CP::ResourceOrTag, CharFmtBuilder().fg("#94A585")->italic(true)->ok() },
    { CP::ResourceOrTagKey,
      CharFmtBuilder().fg("#8CB337")->italic(true)->ok() },
    { CP::Rotation, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::ScoreboardSlot, CharFmtBuilder().fg("#7d364b")->ok() },
    { CP::Style, CharFmtBuilder().bg("#aa45001E")->ok() },
    { CP::Swizzle, CharFmtBuilder().fg("#de59ce")->ok() },
    { CP::Team, CharFmtBuilder().fg("#826c4f")->ok() },
    { CP::TemplateMirror, CharFmtBuilder().fg("#E5E7CB")->ok() },
    { CP::TemplateRotation, CharFmtBuilder().fg("#B1C928")->ok() },
    { CP::Time, CharFmtBuilder().fg("#5C37A5")->ok() },
    { CP::Uuid, CharFmtBuilder().fg("#9c442f")->ok() },
    { CP::Vec2, CharFmtBuilder().fg("#8934cf")->ok() },
    { CP::Vec3, CharFmtBuilder().fg("#8c54ba")->ok() },
    { CP::Comment, CharFmtBuilder().fg("#577D45")->ok() },
    { CP::Number, CharFmtBuilder().fg("#A2D4E8")->ok() },
    { CP::Directive, CharFmtBuilder().fg("#DC785A")->bold(true)->ok() },
    { CP::Script, CharFmtBuilder().bg("#aa494949")->ok() },
    { CP::QuotedString, CharFmtBuilder().fg("#b84f58")->ok() },
    { CP::CommandLiteral,
      CharFmtBuilder().fg(59, 120, 255)->bold(true)->ok() },
    { CP::BlockState_States, CharFmtBuilder().bg("#aa341123")->ok() },
    { CP::NbtPath_Step, CharFmtBuilder().bg("#aa452400")->ok() },
    { CP::TargetSelector_Variable, CharFmtBuilder().fg("#28C716")->weight(
          QFont::DemiBold)->ok() },
    { CP::TargetSelector_Arguments, CharFmtBuilder().bg("#aa341123")->ok() },
    { CP::ItemStack_Components, CharFmtBuilder().bg("#aa551337")->ok() },
    { CP::Key, CharFmtBuilder().fg("#B89C3D")->ok() },
    { CP::Gamemode, CharFmtBuilder().fg("#BBC6F1")->bold(true)->ok() },
    { CP::RegexPattern, CharFmtBuilder().fg("#b4b4b4")->ok() },
    { CP::GreedyString, CharFmtBuilder().fg("#B72E3A")->ok() },
};
