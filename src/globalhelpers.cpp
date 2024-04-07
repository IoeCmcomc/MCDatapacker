#include "globalhelpers.h"

#include "uberswitch.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QStringMatcher>

using namespace Glhp;

QChar Glhp::randChr(QStringView charset) {
    return charset.at(QRandomGenerator::global()->bounded((int)charset.size()));
}

QString Glhp::randStr(QStringView charset, int length) {
    QString result;

    for (int i = 0; i < length; ++i) {
        const int index = QRandomGenerator::global()->bounded(
            (int)charset.size());
        result.append(charset.at(index));
    }
    return result;
}

QString Glhp::randStr(int length) {
    static const QString charset = QStringLiteral(
        "abcdefghijklmnopqrstuvwxyz0123456789");

    return randStr(charset, length);
}

QString Glhp::relPath(const QString &dirpath, QString path) {
    const QString &&dataDir = dirpath + QLatin1Char('/');

    if (!removePrefix(path, dataDir))
        return QString();

    return path;
}

QString Glhp::relNamespace(const QString &dirpath, QString path) {
    QString &&rp = relPath(dirpath, std::move(path));

    if (removePrefix(rp, "data/"_QL1))
        rp = rp.section('/', 0, 0);
    else
        rp.clear();
    return std::move(rp);
}

CodeFile::FileType Glhp::pathToFileType(const QString &dirpath,
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
                if (isPathRelativeTo(dirpath, filepath, pair.first)) {
                    return pair.second;
                }
            }
            return CodeFile::JsonText;
        }

        ucase (QLatin1String("txt")):
            return isPathRelativeTo(dirpath, filepath, u"jmc.txt")
                ? CodeFile::JmcCert : CodeFile::Text;
    }
    return CodeFile::Text;
}

QIcon Glhp::fileTypeToIcon(const CodeFile::FileType type) {
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

bool Glhp::isPathRelativeTo(const QString &dirpath, QStringView path,
                            QStringView category) {
    static QString && dataDir{};
    dataDir = dirpath + "/data/"_QL1;

    if (!path.startsWith(dataDir))
        return false;

    const int sepIndex = path.indexOf('/', dataDir.length() + 1);
    if (sepIndex == -1)
        return false;

    return path.mid(sepIndex + 1).startsWith(category);
}

QString Glhp::toNamespacedID(const QString &dirpath, QStringView filepath,
                             bool noTagForm) {
    const QString &&dataDir = dirpath + QStringLiteral("/data/");

    if (!filepath.startsWith(dataDir))
        return QString();

    static QStringMatcher sepMatcher(u"/");

    // Extract namespace part
    int sepIndex = sepMatcher.indexIn(filepath, dataDir.length() + 1);
    if (sepIndex == -1)
        return QString();

    const QString &&nameSpace = filepath.mid(dataDir.length(),
                                             sepIndex -
                                             dataDir.length()).toString();

    // Extract ID part
    bool isTag = false;
    if (filepath.mid(sepIndex).startsWith("/tags/"_QL1)) {
        isTag    = true;
        sepIndex = sepMatcher.indexIn(filepath, sepIndex + 1);
        if (sepIndex == -1)
            return QString();
    }

    if (filepath.mid(sepIndex).startsWith("/worldgen/"_QL1)) {
        sepIndex = sepMatcher.indexIn(filepath, sepIndex + 1);
        if (sepIndex == -1)
            return QString();
    }

    sepIndex = sepMatcher.indexIn(filepath, sepIndex + 1);
    if (sepIndex == -1)
        return QString();

    const int       lastDot = filepath.lastIndexOf('.');
    const QString &&id      =
        filepath.mid(sepIndex + 1, lastDot - sepIndex - 1).toString();

    if (isTag && !noTagForm) {
        return "#"_QL1 + nameSpace + ":"_QL1 + id;
    } else {
        return nameSpace + ":"_QL1 + id;
    }
}

QVariant Glhp::strToVariant(QStringView str) {
    bool isInt;
    auto intValue = str.toInt(&isInt);

    if (isInt) {
        return intValue;
    } else if (str == QLatin1String("true")) {
        return true;
    } else if (str == QLatin1String("false")) {
        return false;
    } else {
        return str.toString();
    }
}

QString Glhp::variantToStr(const QVariant &vari) {
    if (vari.type() == QVariant::Bool)
        return vari.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    else
        return vari.toString();
}

QVector<QString> Glhp::fileIdList(const QString &dirpath, const QString &catDir,
                                  const QString &nspace, bool noTagForm) {
    QVector<QString> idList;
    const QString  &&dataPath = dirpath + QStringLiteral("/data/");

    auto &&appendPerCategory =
        [noTagForm, &dirpath, &dataPath, &idList](const QString &nspace,
                                                  const QString &catDir)->void {
            QDir idDir(dataPath + nspace + '/' + catDir);

            if (idDir.exists() && (!idDir.isEmpty())) {
                QDirIterator it(idDir.path(),
                                { "*.mcfunction", "*.json", "*.nbt" },
                                QDir::Files | QDir::NoDotAndDotDot,
                                QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    idList << toNamespacedID(dirpath, it.next(), noTagForm);
                }
            }
        };

    auto &&appendIDToList =
        [appendPerCategory](const QString &nspace,
                            const QString &catDir)->void {
            if (!catDir.isEmpty()) {
                appendPerCategory(nspace, catDir);
            } else {
                appendPerCategory(nspace, "advancements");
                appendPerCategory(nspace, "functions");
                appendPerCategory(nspace, "loot_tables");
                appendPerCategory(nspace, "predicates");
                appendPerCategory(nspace, "item_modifiers");
                appendPerCategory(nspace, "recipes");
                appendPerCategory(nspace, "dimension");
                appendPerCategory(nspace, "dimension_type");
                appendPerCategory(nspace, "chat_type");
                appendPerCategory(nspace, "trim_material");
                appendPerCategory(nspace, "trim_pattern");
                appendPerCategory(nspace, "tag");
                appendPerCategory(nspace, "worldgen");
            }
        };


    if (nspace.isEmpty()) {
        QDir         dir(dataPath);
        const auto &&nspaceDirs = dir.entryList(
            QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &nspaceDir : nspaceDirs) {
            appendIDToList(nspaceDir.section('.', 0, 0), catDir);
        }
    } else {
        appendIDToList(nspace, catDir);
    }
    return idList;
}

bool Glhp::removePrefix(QString &str, QLatin1String prefix) {
    bool &&r = str.startsWith(prefix);

    if (r)
        str.remove(0, prefix.size());
    return r;
}

bool Glhp::removePrefix(QString &str, QStringView prefix) {
    bool &&r = str.startsWith(prefix);

    if (r)
        str.remove(0, prefix.length());
    return r;
}

const QMap<QString, QString> Glhp::colorHexes = {
    { "black",        "#000000" },
    { "dark_blue",    "#0000aa" },
    { "dark_green",   "#00aa00" },
    { "dark_aqua",    "#00aaaa" },
    { "dark_red",     "#aa0000" },
    { "dark_purple",  "#aa00aa" },
    { "gold",         "#ffaa00" },
    { "gray",         "#aaaaaa" },
    { "dark_gray",    "#555555" },
    { "blue",         "#5555ff" },
    { "green",        "#55ff55" },
    { "aqua",         "#55ffff" },
    { "red",          "#ff5555" },
    { "light_purple", "#ff55ff" },
    { "yellow",       "#ffff55" },
    { "white",        "#ffffff" }
};

const QMap<char, QString> Glhp::colorCodes = {
    { '0', "#000000" },
    { '1', "#0000aa" },
    { '2', "#00aa00" },
    { '3', "#00aaaa" },
    { '4', "#aa0000" },
    { '5', "#aa00aa" },
    { '6', "#ffaa00" },
    { '7', "#aaaaaa" },
    { '8', "#555555" },
    { '9', "#5555ff" },
    { 'a', "#55ff55" },
    { 'b', "#55ffff" },
    { 'c', "#ff5555" },
    { 'd', "#ff55ff" },
    { 'e', "#ffff55" },
    { 'f', "#ffffff" }
};


QString Glhp::fileTypeToName(const CodeFile::FileType type) {
    static QHash<CodeFile::FileType, const char *> const valueMap {
        { CodeFile::Binary, QT_TRANSLATE_NOOP("Glhp", "Binary") },
        { CodeFile::Structure, QT_TRANSLATE_NOOP("Glhp", "Structure") },
        { CodeFile::Image, QT_TRANSLATE_NOOP("Glhp", "Image") },
        { CodeFile::Text, QT_TRANSLATE_NOOP("Glhp", "Other") },
        { CodeFile::Function, QT_TRANSLATE_NOOP("Glhp", "Function") },
        { CodeFile::JsonText, QT_TRANSLATE_NOOP("Glhp", "JSON") },
        { CodeFile::Advancement, QT_TRANSLATE_NOOP("Glhp", "Advancement") },
        { CodeFile::LootTable, QT_TRANSLATE_NOOP("Glhp", "Loot table") },
        { CodeFile::Meta, QT_TRANSLATE_NOOP("Glhp", "Information") },
        { CodeFile::Predicate, QT_TRANSLATE_NOOP("Glhp", "Predicate") },
        { CodeFile::ItemModifier, QT_TRANSLATE_NOOP("Glhp", "Item modifier") },
        { CodeFile::Recipe, QT_TRANSLATE_NOOP("Glhp", "Recipe") },
        { CodeFile::Dimension, QT_TRANSLATE_NOOP("Glhp", "Dimension") },
        { CodeFile::DimensionType,
          QT_TRANSLATE_NOOP("Glhp", "Dimension type") },
        { CodeFile::ChatType, QT_TRANSLATE_NOOP("Glhp", "Chat type") },
        { CodeFile::DamageType, QT_TRANSLATE_NOOP("Glhp", "Damage type") },
        { CodeFile::TrimMaterial, QT_TRANSLATE_NOOP("Glhp", "Trim material") },
        { CodeFile::TrimPattern, QT_TRANSLATE_NOOP("Glhp", "Trim pattern") },
        { CodeFile::BlockTag, QT_TRANSLATE_NOOP("Glhp", "Block tag") },
        { CodeFile::EntityTypeTag,
          QT_TRANSLATE_NOOP("Glhp", "Entity type tag") },
        { CodeFile::FluidTag, QT_TRANSLATE_NOOP("Glhp", "Fluid tag") },
        { CodeFile::FunctionTag, QT_TRANSLATE_NOOP("Glhp", "Function tag") },
        { CodeFile::ItemTag, QT_TRANSLATE_NOOP("Glhp", "Item tag") },
        { CodeFile::GameEventTag, QT_TRANSLATE_NOOP("Glhp", "Game event tag") },
        { CodeFile::Tag, QT_TRANSLATE_NOOP("Glhp", "Tag") },
        { CodeFile::WorldGen, QT_TRANSLATE_NOOP("Glhp", "World generation") },
        { CodeFile::Biome, QT_TRANSLATE_NOOP("Glhp", "Biome") },
        { CodeFile::ConfiguredCarver, QT_TRANSLATE_NOOP("Glhp", "Carver") },
        { CodeFile::ConfiguredFeature, QT_TRANSLATE_NOOP("Glhp", "Feature") },
        { CodeFile::StructureFeature, QT_TRANSLATE_NOOP("Glhp",
                                                        "Structure feature") },
        { CodeFile::SurfaceBuilder,
          QT_TRANSLATE_NOOP("Glhp", "Surface builder") },
        { CodeFile::Noise, QT_TRANSLATE_NOOP("Glhp", "Noise") },
        { CodeFile::NoiseSettings,
          QT_TRANSLATE_NOOP("Glhp", "Noise settings") },
        { CodeFile::PlacedFeature,
          QT_TRANSLATE_NOOP("Glhp", "Placed feature") },
        { CodeFile::ProcessorList,
          QT_TRANSLATE_NOOP("Glhp", "Processor list") },
        { CodeFile::StructureSet, QT_TRANSLATE_NOOP("Glhp", "Structure set") },
        { CodeFile::TemplatePool, QT_TRANSLATE_NOOP("Glhp", "Jigsaw pool") },
        { CodeFile::FlatLevelGenPreset,
          QT_TRANSLATE_NOOP("Glhp", "Flat world generator preset") },
        { CodeFile::Jmc,
          QT_TRANSLATE_NOOP("Glhp", "JavaScript-like Minecraft function") },
        { CodeFile::JmcHeader, QT_TRANSLATE_NOOP("Glhp", "JMC header") },
        { CodeFile::JmcCert, QT_TRANSLATE_NOOP("Glhp", "JMC certificate") },
        { CodeFile::McBuild, QT_TRANSLATE_NOOP("Glhp", "mc-build code") },
        { CodeFile::McBuildMacro, QT_TRANSLATE_NOOP("Glhp", "mc-build macro") },
        // Do not translate "Trident"
        { CodeFile::TridentCode, QT_TRANSLATE_NOOP("Glhp", "Trident code") },
    };

    if (type < 0)
        return QString();

    return QCoreApplication::translate("Glhp", valueMap[type]);
}

bool Glhp::removeInternalPrefix(QString &path) {
    if (removePrefix(path, ":/minecraft/"_QL1)) {
        const int index = path.indexOf('/');
        path.remove(0, index + 1);
        return true;
    } else {
        return false;
    }
}
