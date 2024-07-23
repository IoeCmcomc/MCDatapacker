#include "globalhelpers.h"

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

QVector<QString> Glhp::fileIdList(const QString &dirpath,
                                  const QString &catDir,
                                  const QString &nspace,
                                  bool noTagForm,
                                  bool from_1_21) {
    QVector<QString> idList;
    const QString  &&dataPath = dirpath + QStringLiteral("/data/");

    auto &&appendPerCategory =
        [noTagForm, &dirpath, &dataPath, &idList, &from_1_21]
        (const QString &nspace, const QString &catDir)->void {
            QDir idDir(dataPath + nspace + '/' +
                       canonicalCategory(catDir, from_1_21));

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


bool Glhp::removeInternalPrefix(QString &path) {
    if (removePrefix(path, ":/minecraft/"_QL1)) {
        const int index = path.indexOf('/');
        path.remove(0, index + 1);
        return true;
    } else {
        return false;
    }
}

const static QSet<QStringView> PLURAL_CATEGORY_VIEWS{
    u"advancements", u"functions", u"item_modifiers", u"loot_tables",
    u"recipes", u"structures", u"tags/blocks", u"tags/entity_types",
    u"tags/fluids", u"tags/functions", u"tags/game_events", u"tags/items"
};

QString Glhp::canonicalCategory(QStringView catDir, const bool from_1_21) {
    if (from_1_21 && PLURAL_CATEGORY_VIEWS.contains(catDir)) {
        catDir.chop(1);
    }

    return catDir.toString();
}

QStringView Glhp::canonicalCategoryView(QStringView catDir,
                                        const bool from_1_21) {
    if (from_1_21 && PLURAL_CATEGORY_VIEWS.contains(catDir)) {
        catDir.chop(1);
    }

    return catDir;
}
