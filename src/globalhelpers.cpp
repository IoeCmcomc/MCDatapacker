#include "globalhelpers.h"

#include "mainwindow.h"
#include "vieweventfilter.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QRandomGenerator>

using namespace Glhp;

QString Glhp::randStr(int length) {
    static const QLatin1String charset("abcdefghijklmnopqrstuvwxyz0123456789");
    QString                    r;

    for (int i = 0; i < (length + 1); ++i) {
        int index = QRandomGenerator::global()->bounded(charset.size());
        r.append(charset.at(index));
    }
    return r;
}

QString Glhp::relPath(const QString &dirpath, QString path) {
    const QString &&dataDir = dirpath + "/";

    if (!removePrefix(path, dataDir))
        return QString();

    return path;
}

QString Glhp::relNamespace(const QString &dirpath, QString path) {
    QString &&rp = relPath(dirpath, std::move(path));

    if (removePrefix(rp, QStringLiteral("data/")))
        rp = rp.section('/', 0, 0);
    else
        rp.clear();
    return std::move(rp);
}

CodeFile::FileType Glhp::pathToFileType(const QString &dirpath,
                                        const QString &filepath) {
    if (filepath.isEmpty())
        return CodeFile::Text;

    static const QStringList imageExts =
    { QStringLiteral("png"), QStringLiteral("jpg"), QStringLiteral("jpeg"),
      QStringLiteral("bmp") };

    QFileInfo       info(filepath);
    const QString &&fullSuffix = info.suffix();

    if (fullSuffix == QLatin1String("mcmeta")) {
        return CodeFile::Meta;
    } else if (fullSuffix == QLatin1String("mcfunction")) {
        return CodeFile::Function;
    } else if (fullSuffix == QLatin1String("nbt")) {
        return CodeFile::Structure;
    } else if (imageExts.contains(fullSuffix)) {
        return CodeFile::Image;
    } else if (fullSuffix == QLatin1String("json")) {
        if (isPathRelativeTo(dirpath, filepath,
                             QStringLiteral("advancements"))) {
            return CodeFile::Advancement;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("loot_tables"))) {
            return CodeFile::LootTable;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("predicates"))) {
            return CodeFile::Predicate;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("item_modifiers"))) {
            return CodeFile::ItemModifier;
        } else if (isPathRelativeTo(dirpath, filepath, QStringLiteral(
                                        "recipes"))) {
            return CodeFile::Recipe;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("tags/blocks"))) {
            return CodeFile::BlockTag;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("tags/entity_types"))) {
            return CodeFile::EntityTypeTag;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("tags/fluids"))) {
            return CodeFile::FluidTag;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("tags/functions"))) {
            return CodeFile::FunctionTag;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("tags/items"))) {
            return CodeFile::ItemTag;
        } else {
            return CodeFile::JsonText;
        }
    } else {
        return CodeFile::Text;
    }
}

QIcon Glhp::fileTypeToIcon(const CodeFile::FileType type) {
    switch (type) {
    case CodeFile::Function:
        return QIcon(QStringLiteral(":/file-mcfunction"));

    case CodeFile::Structure:
        return QIcon(QStringLiteral(":/file-nbt"));

    case CodeFile::Meta:
        return QIcon(QStringLiteral(":/file-mcmeta"));

    default: {
        if (type >= CodeFile::JsonText)
            return QIcon(QStringLiteral(":/file-json"));

        break;
    }
    }
    return QIcon();
}

bool Glhp::isPathRelativeTo(const QString &dirpath,
                            const QString &path,
                            const QString &catDir) {
    const auto &&tmpDir = dirpath + QStringLiteral("/data/");

    if (!path.startsWith(tmpDir)) return false;

    return path.mid(tmpDir.length()).section('/', 1).startsWith(catDir);
}

QString Glhp::toNamespacedID(const QString &dirpath,
                             QString filepath,
                             bool noTagForm) {
    const QString &&datapath = dirpath + QStringLiteral("/data/");
    QString         r;

    if (filepath.startsWith(datapath)) {
        const auto &&finfo = QFileInfo(filepath);
        filepath = finfo.dir().path() + '/' + finfo.completeBaseName();
        filepath.remove(0, datapath.length());
        if (!noTagForm && isPathRelativeTo(dirpath, finfo.filePath(),
                                           QStringLiteral("tags"))) {
            if (filepath.split('/').count() >= 4) {
                r = "#" + filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 3);
            }
        } else {
            if (filepath.split('/').count() >= 3) {
                r = filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 2);
            }
        }
    }
    return r;
}

QVariant Glhp::strToVariant(const QString &str) {
    bool isInt;
    auto intValue = str.toInt(&isInt);

    if (isInt) {
        return intValue;
    } else {
        if (str == QLatin1String("true") || str == QLatin1String("false")) {
            return str == QLatin1String("true");
        } else {
            return str;
        }
    }
}

QString Glhp::variantToStr(const QVariant &vari) {
    if (vari.type() == QVariant::Bool)
        return vari.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    else if (vari.type() == QVariant::Int)
        return vari.toString();
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
            }
        };


    if (nspace.isEmpty()) {
        QDir   dir(dataPath);
        auto &&nspaceDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &nspaceDir : nspaceDirs) {
            appendIDToList(nspaceDir.section('.', 0, 0), catDir);
        }
    } else {
        appendIDToList(nspace, catDir);
    }
    return idList;
}

bool Glhp::removePrefix(QString &str, const QString &prefix) {
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
    { "glod",         "#ffAA00" },
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


QString Glhp::fileTypeToName(const CodeFile::FileType type) {
    static QHash<CodeFile::FileType, const char*> const valueMap = {
        { CodeFile::Binary,        QT_TR_NOOP("Binary")          },
        { CodeFile::Structure,     QT_TR_NOOP("Structure")       },
        { CodeFile::Image,         QT_TR_NOOP("Image")           },
        { CodeFile::Text,          QT_TR_NOOP("Other")           },
        { CodeFile::Function,      QT_TR_NOOP("Function")        },
        { CodeFile::JsonText,      QT_TR_NOOP("JSON")            },
        { CodeFile::Advancement,   QT_TR_NOOP("Advancement")     },
        { CodeFile::LootTable,     QT_TR_NOOP("Loot table")      },
        { CodeFile::Meta,          QT_TR_NOOP("Information")     },
        { CodeFile::Predicate,     QT_TR_NOOP("Predicate")       },
        { CodeFile::ItemModifier,  QT_TR_NOOP("Item modifier")   },
        { CodeFile::Recipe,        QT_TR_NOOP("Recipe")          },
        { CodeFile::BlockTag,      QT_TR_NOOP("Block tag")       },
        { CodeFile::EntityTypeTag, QT_TR_NOOP("Entity type tag") },
        { CodeFile::FluidTag,      QT_TR_NOOP("Fluid tag")       },
        { CodeFile::FunctionTag,   QT_TR_NOOP("Function tag")    },
        { CodeFile::ItemTag,       QT_TR_NOOP("Item tag")        },
    };

    if (type < 0)
        return QString();

    return QApplication::translate("Glhp", valueMap[type]);
}
