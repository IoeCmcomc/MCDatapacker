#include "globalhelpers.h"

#include "mainwindow.h"
#include "vieweventfilter.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>

using namespace Glhp;

QString Glhp::randStr(int length) {
    static const QString charset("abcdefghijklmnopqrstuvwxyz0123456789");
    QString              r;

    for (int i = 0; i < (length + 1); ++i) {
        int index = QRandomGenerator::global()->bounded(charset.length());
        r.append(charset.at(index));
    }
    return r;
}

QString Glhp::relPath(const QString &dirpath, QString path) {
    QString dataDir = dirpath + "/";

    if (!removePrefix(path, dataDir))
        return QString();

    return path;
}

QString Glhp::relNamespace(const QString &dirpath, QString path) {
    QString rp = relPath(dirpath, std::move(path));

    if (removePrefix(rp, QStringLiteral("data/")))
        rp = rp.section('/', 0, 0);
    else
        rp = "";
    return rp;
}

CodeFile::FileType Glhp::pathToFileType(const QString &dirpath,
                                        const QString &filepath) {
    if (filepath.isEmpty())
        return CodeFile::Text;

    QFileInfo                info       = QFileInfo(filepath);
    const QString            fullSuffix = info.suffix();
    static const QStringList imageExts  =
    { QStringLiteral("png"), QStringLiteral("jpg"), QStringLiteral("jpeg"),
      QStringLiteral("bmp") };

    if (fullSuffix == QStringLiteral("mcmeta")) {
        return CodeFile::Meta;
    } else if (fullSuffix == QStringLiteral("mcfunction")) {
        return CodeFile::Function;
    } else if (fullSuffix == QStringLiteral("nbt")) {
        return CodeFile::Structure;
    } else if (imageExts.contains(fullSuffix)) {
        return CodeFile::Image;
    } else if (fullSuffix == QStringLiteral("json")) {
        if (isPathRelativeTo(dirpath, filepath,
                             QStringLiteral("advancements"))) {
            return CodeFile::Advancement;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("loot_tables"))) {
            return CodeFile::LootTable;
        } else if (isPathRelativeTo(dirpath, filepath,
                                    QStringLiteral("predicates"))) {
            return CodeFile::Predicate;
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

QString Glhp::toNamespacedID(const QString &dirpath, QString filepath) {
    const QString &&datapath = dirpath + QStringLiteral("/data/");
    QString         r;

    if (filepath.startsWith(datapath)) {
        const auto &&finfo = QFileInfo(filepath);
        filepath = finfo.dir().path() + '/' + finfo.completeBaseName();
        filepath.remove(0, datapath.length());
        if (isPathRelativeTo(dirpath, finfo.filePath(),
                             QStringLiteral("tags"))) {
            if (filepath.split('/').count() >= 4)
                r = "#" + filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 3);
        } else {
            if (filepath.split('/').count() >= 3)
                r = filepath.section('/', 0, 0)
                    + ':' + filepath.section('/', 2);
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
        if (str == QStringLiteral("true") || str == QStringLiteral("false")) {
            return str == QStringLiteral("true");
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
                                  const QString &nspace) {
    QVector<QString> idList;
    const QString  &&dataPath = dirpath + QStringLiteral("/data/");

    auto &&appendPerCategory =
        [&dataPath, &idList](const QString &nspace,
                             const QString &catDir)->void {
            ;
            QDir IDDir(dataPath + nspace + '/' + catDir);

            if (IDDir.exists() && (!IDDir.isEmpty())) {
                auto &&names = IDDir.entryList(
                    QDir::Files | QDir::NoDotAndDotDot);
                for (auto &&name : names) {
                    name = name.section('.', 0, 0);
                    idList.push_back(nspace + ":" +
                                     name);
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
                appendPerCategory(nspace, "recipes");
            }
        };


    if (nspace.isEmpty()) {
        QDir   dir(dataPath);
        auto &&nspaceDirs = dir.entryList(
            QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &nspaceDir : nspaceDirs) {
            auto nspace = nspaceDir.section('.', 0, 0);
            appendIDToList(nspace, catDir);
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
