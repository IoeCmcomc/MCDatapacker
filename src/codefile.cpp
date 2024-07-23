#include "codefile.h"

#include "globalhelpers.h"
#include "uberswitch.hpp"
#include "game.h"

#include <QCoreApplication>


CodeFile::CodeFile(const QString &path) {
    changePath(path);
}

QString CodeFile::name() const {
    return info.fileName();
}

QString CodeFile::path() const {
    return info.filePath();
}

void CodeFile::changePath(const QString &path) {
    info = QFileInfo(path);
    QString newPath = path;
    if (Glhp::removeInternalPrefix(newPath)) {
        fileType =
            pathToFileType(QStringLiteral("data-json"), newPath);
    } else if (info.isFile()) {
        fileType = pathToFileType(QDir::currentPath(), path);
    } else {
        qWarning() << "Non-file path:" << path;
    }
}

CodeFile::FileType CodeFile::pathToFileType(const QString &dirpath,
                                            const QString &filepath) {
    if (filepath.isEmpty())
        return CodeFile::Text;

    static const QVector<QPair<QString,
                               CodeFile::FileType> > catPathToFileType {
        { QStringLiteral("advancements"), CodeFile::Advancement },
        { QStringLiteral("loot_tables"), CodeFile::LootTable },
        { QStringLiteral("predicates"), CodeFile::Predicate },
        { QStringLiteral("item_modifiers"), CodeFile::ItemModifier },
        { QStringLiteral("recipes"), CodeFile::Recipe },
        { QStringLiteral("dimension"), CodeFile::Dimension },
        { QStringLiteral("dimension_type"), CodeFile::DimensionType },
        { QStringLiteral("chat_type"), CodeFile::ChatType },
        { QStringLiteral("damage_type"), CodeFile::DamageType },
        { QStringLiteral("trim_material"), CodeFile::TrimMaterial },
        { QStringLiteral("trim_pattern"), CodeFile::TrimPattern },
        { QStringLiteral("tags/blocks"), CodeFile::BlockTag },
        { QStringLiteral("tags/entity_types"), CodeFile::EntityTypeTag },
        { QStringLiteral("tags/fluids"), CodeFile::FluidTag },
        { QStringLiteral("tags/functions"), CodeFile::FunctionTag },
        { QStringLiteral("tags/items"), CodeFile::ItemTag },
        { QStringLiteral("tags/game_events"), CodeFile::GameEventTag },
        { QStringLiteral("tags"), CodeFile::Tag },
        { QStringLiteral("worldgen/biome"), CodeFile::Biome },
        { QStringLiteral("worldgen/configured_carver"),
          CodeFile::ConfiguredCarver },
        { QStringLiteral("worldgen/configured_feature"),
          CodeFile::ConfiguredFeature },
        { QStringLiteral("worldgen/configured_structure_feature"),
          CodeFile::StructureFeature },
        { QStringLiteral("worldgen/configured_surface_builder"),
          CodeFile::SurfaceBuilder },
        { QStringLiteral("worldgen/flat_level_generator_preset"),
          CodeFile::FlatLevelGenPreset },
        { QStringLiteral("worldgen/noise"), CodeFile::Noise },
        { QStringLiteral("worldgen/noise_settings"), CodeFile::NoiseSettings },
        { QStringLiteral("worldgen/placed_feature"), CodeFile::PlacedFeature },
        { QStringLiteral("worldgen/processor_list"), CodeFile::ProcessorList },
        { QStringLiteral("worldgen/structure_set"), CodeFile::StructureSet },
        { QStringLiteral("worldgen/template_pool"), CodeFile::TemplatePool },
        { QStringLiteral("worldgen/structure"), CodeFile::StructureFeature },
        { QStringLiteral("worldgen"), CodeFile::WorldGen },
    };

    const QFileInfo info(filepath);
    const QString &&fullSuffix = info.suffix();

    uswitch (fullSuffix) {
        ucase (QLatin1String("mcmeta")):
            return CodeFile::Meta;

        ucase (QLatin1String("mcfunction")):
            return CodeFile::Function;

        ucase (QLatin1String("nbt")):
            return CodeFile::Structure;

        ucase (QLatin1String("png")):
        ucase (QLatin1String("jpg")):
        ucase (QLatin1String("jpeg")):
        ucase (QLatin1String("bmp")):
            return CodeFile::Image;

        ucase (QLatin1String("mc")):
        ucase (QLatin1String("mcb")):
            return CodeFile::McBuild;

        ucase (QLatin1String("mcm")):
        ucase (QLatin1String("mcbm")):
            return CodeFile::McBuildMacro;

        ucase (QLatin1String("jmc")):
            return CodeFile::Jmc;

        ucase (QLatin1String("hjmc")):
            return CodeFile::JmcHeader;

        ucase (QLatin1String("tdn")):
            return CodeFile::TridentCode;

        ucase (QLatin1String("json")): {
            for (const auto &pair: catPathToFileType) {
                if (Glhp::isPathRelativeTo(dirpath, filepath,
                                           Game::canonicalCategory(pair.first)))
                {
                    return pair.second;
                }
            }
            return CodeFile::JsonText;
        }

        ucase (QLatin1String("txt")):
            return Glhp::isPathRelativeTo(dirpath, filepath, u"jmc.txt")
            ? CodeFile::JmcCert : CodeFile::Text;
    }
    return CodeFile::Text;
}

QIcon CodeFile::fileTypeToIcon(const CodeFile::FileType type) {
    using IconCache = QMap<CodeFile::FileType, QIcon>;
    static IconCache cache;

    auto it = cache.constFind(type);
    if (it != cache.cend()) {
        return it.value();
    }

    QIcon icon;
    switch (type) {
        case CodeFile::Function: {
            icon.addPixmap(QStringLiteral(":/file-mcfunction"));
            break;
        }
        case CodeFile::Structure: {
            icon.addPixmap(QStringLiteral(":/file-nbt"));
            break;
        }        case CodeFile::Meta: {
            icon.addPixmap(QStringLiteral(":/file-mcmeta"));
            break;
        }
        case CodeFile::McBuild: {
            icon.addPixmap(QStringLiteral(":/file-mc"));
            break;
        }
        case CodeFile::McBuildMacro: {
            icon.addPixmap(QStringLiteral(":/file-mcm"));
            break;
        }
        case CodeFile::Jmc: {
            icon.addPixmap(QStringLiteral(":/file-jmc"));
            break;
        }
        case CodeFile::JmcHeader: {
            icon.addPixmap(QStringLiteral(":/file-hjmc"));
            break;
        }
        case CodeFile::JmcCert: {
            icon.addPixmap(QStringLiteral(":/file-jmc.txt"));
            break;
        }
        case CodeFile::TridentCode: {
            icon.addPixmap(QStringLiteral(":/file-tdn"));
            break;
        }
        default: {
            if ((type >= CodeFile::JsonText) && (type < CodeFile::JsonText_end))
                icon.addPixmap(QStringLiteral(":/file-json"));

            break;
        }
    }

    cache.insert(it, type, icon);
    return icon;
}

QString CodeFile::fileTypeToName(const CodeFile::FileType type) {
    static QHash<CodeFile::FileType, const char *> const valueMap {
        { CodeFile::Binary, QT_TRANSLATE_NOOP("CodeFile", "Binary") },
        { CodeFile::Structure, QT_TRANSLATE_NOOP("CodeFile", "Structure") },
        { CodeFile::Image, QT_TRANSLATE_NOOP("CodeFile", "Image") },
        { CodeFile::Text, QT_TRANSLATE_NOOP("CodeFile", "Other") },
        { CodeFile::Function, QT_TRANSLATE_NOOP("CodeFile", "Function") },
        { CodeFile::JsonText, QT_TRANSLATE_NOOP("CodeFile", "JSON") },
        { CodeFile::Advancement, QT_TRANSLATE_NOOP("CodeFile", "Advancement") },
        { CodeFile::LootTable, QT_TRANSLATE_NOOP("CodeFile", "Loot table") },
        { CodeFile::Meta, QT_TRANSLATE_NOOP("CodeFile", "Information") },
        { CodeFile::Predicate, QT_TRANSLATE_NOOP("CodeFile", "Predicate") },
        { CodeFile::ItemModifier,
          QT_TRANSLATE_NOOP("CodeFile", "Item modifier") },
        { CodeFile::Recipe, QT_TRANSLATE_NOOP("CodeFile", "Recipe") },
        { CodeFile::Dimension, QT_TRANSLATE_NOOP("CodeFile", "Dimension") },
        { CodeFile::DimensionType,
          QT_TRANSLATE_NOOP("CodeFile", "Dimension type") },
        { CodeFile::ChatType, QT_TRANSLATE_NOOP("CodeFile", "Chat type") },
        { CodeFile::DamageType, QT_TRANSLATE_NOOP("CodeFile", "Damage type") },
        { CodeFile::TrimMaterial,
          QT_TRANSLATE_NOOP("CodeFile", "Trim material") },
        { CodeFile::TrimPattern,
          QT_TRANSLATE_NOOP("CodeFile", "Trim pattern") },
        { CodeFile::BlockTag, QT_TRANSLATE_NOOP("CodeFile", "Block tag") },
        { CodeFile::EntityTypeTag,
          QT_TRANSLATE_NOOP("CodeFile", "Entity type tag") },
        { CodeFile::FluidTag, QT_TRANSLATE_NOOP("CodeFile", "Fluid tag") },
        { CodeFile::FunctionTag,
          QT_TRANSLATE_NOOP("CodeFile", "Function tag") },
        { CodeFile::ItemTag, QT_TRANSLATE_NOOP("CodeFile", "Item tag") },
        { CodeFile::GameEventTag,
          QT_TRANSLATE_NOOP("CodeFile", "Game event tag") },
        { CodeFile::Tag, QT_TRANSLATE_NOOP("CodeFile", "Tag") },
        { CodeFile::WorldGen,
          QT_TRANSLATE_NOOP("CodeFile", "World generation") },
        { CodeFile::Biome, QT_TRANSLATE_NOOP("CodeFile", "Biome") },
        { CodeFile::ConfiguredCarver, QT_TRANSLATE_NOOP("CodeFile", "Carver") },
        { CodeFile::ConfiguredFeature,
          QT_TRANSLATE_NOOP("CodeFile", "Feature") },
        { CodeFile::StructureFeature, QT_TRANSLATE_NOOP("CodeFile",
                                                        "Structure feature") },
        { CodeFile::SurfaceBuilder,
          QT_TRANSLATE_NOOP("CodeFile", "Surface builder") },
        { CodeFile::Noise, QT_TRANSLATE_NOOP("CodeFile", "Noise") },
        { CodeFile::NoiseSettings,
          QT_TRANSLATE_NOOP("CodeFile", "Noise settings") },
        { CodeFile::PlacedFeature,
          QT_TRANSLATE_NOOP("CodeFile", "Placed feature") },
        { CodeFile::ProcessorList,
          QT_TRANSLATE_NOOP("CodeFile", "Processor list") },
        { CodeFile::StructureSet,
          QT_TRANSLATE_NOOP("CodeFile", "Structure set") },
        { CodeFile::TemplatePool,
          QT_TRANSLATE_NOOP("CodeFile", "Jigsaw pool") },
        { CodeFile::FlatLevelGenPreset,
          QT_TRANSLATE_NOOP("CodeFile", "Flat world generator preset") },
        { CodeFile::Jmc,
          QT_TRANSLATE_NOOP("CodeFile", "JavaScript-like Minecraft function") },
        { CodeFile::JmcHeader, QT_TRANSLATE_NOOP("CodeFile", "JMC header") },
        { CodeFile::JmcCert, QT_TRANSLATE_NOOP("CodeFile", "JMC certificate") },
        { CodeFile::McBuild, QT_TRANSLATE_NOOP("CodeFile", "mc-build code") },
        { CodeFile::McBuildMacro,
          QT_TRANSLATE_NOOP("CodeFile", "mc-build macro") },
        // Do not translate "Trident"
        { CodeFile::TridentCode,
          QT_TRANSLATE_NOOP("CodeFile", "Trident code") },
    };

    if (type < 0)
        return QString();

    return QCoreApplication::translate("CodeFile", valueMap[type]);
}

QDebug operator<<(QDebug debug, const CodeFile &file) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "CodeFile(" << file.info << ", "
                    << file.fileType << ')';

    return debug;
}
