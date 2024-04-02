#include "game.h"

#include "lru/lru.hpp"
#include "globalhelpers.h"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QElapsedTimer>
#include <QCborMap>

QVersionNumber Game::version() {
    return QVersionNumber::fromString(versionString());
}

QString Game::versionString() {
    static const QString &&value = QSettings().value(
        QStringLiteral("game/version"), defaultVersionString).toString();

    return value;
}

QVector<QString> Game::getRegistry(const QString &type) {
    return getRegistry(type, Game::versionString());
}

QVector<QString> Game::getRegistry(const QString &type,
                                   const QString &version) {
    static LRU::Cache<std::tuple<QString, QString>,
                      QVector<QString> > cache{ 32 };

    const std::tuple inputPair{ type, version };

    if (cache.contains(inputPair)) return cache.lookup(inputPair);

    const auto &&result = loadRegistry(type, version);

    cache.emplace(std::piecewise_construct,
                  std::forward_as_tuple(type, version),
                  std::forward_as_tuple(result));
    return result;
}

QVector<QString> Game::loadRegistry(const QString &type,
                                    const QString &version) {
    qInfo() << "Game::loadRegistry" << type << version;
    QElapsedTimer timer;
    timer.start();

    const static QString &&filePathTemplate = QStringLiteral(
        ":minecraft/%1/registries/%2/data.min.json");

    QFileInfo finfo(filePathTemplate.arg(version, type));

    if (!finfo.exists() && (version != Game::defaultVersionString)) {
        qWarning() << "Game registry file not exists:" << finfo.filePath()
                   << "; falling back to" << Game::defaultVersionString;
        finfo.setFile(filePathTemplate.arg(Game::defaultVersionString, type));
    }

    if (!finfo.exists()) {
        qWarning() << "Game registry file not exists:" << finfo.filePath()
                   << "; falling back to" << Game::minimumVersionString;
        finfo.setFile(filePathTemplate.arg(Game::minimumVersionString, type));
    }

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "Game registry not exists:" << finfo.filePath()
                   << "; return empty.";
        return {};
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError       errorPtr;
    const QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return {};
    }

    QVector<QString> values;
    const auto     &&array = doc.array();
    for (const auto &value : array) {
        values << value.toString();
    }

    // const static QString &&dirPathTemplate = QStringLiteral(
    //     ":minecraft/%1/data-json");

    // QString realType = type;
    // if (type == "advancement" || type == "loot_table" || type == "recipe") {
    //     realType += u's';
    // }

    // QVector<QString> values =
    //     Glhp::fileIdList(dirPathTemplate.arg(version), realType,
    //                      QStringLiteral("minecraft"));

    qInfo() << "Execution time:" << timer.nsecsElapsed() << "ns";
    return values;
}

QVariantMap Game::getInfo(const QString &type) {
    return getInfo(type, Game::versionString());
}

QVariantMap Game::getInfo(const QString &type,
                          const QString &version) {
    static LRU::Cache<std::tuple<QString, QString>, QVariantMap> cache{ 20 };

    const std::tuple inputPair{ type, version };

    if (cache.contains(inputPair)) return cache.lookup(inputPair);

    const auto &&result = loadInfo(type, version);

    cache.emplace(std::piecewise_construct,
                  std::forward_as_tuple(type, version),
                  std::forward_as_tuple(result));
    return result;
}

QVariantMap Game::loadInfo(const QString &type, const QString &version,
                           const int depth) {
    QVariantMap retMap;

    const static QString &&filePathTemplate = QStringLiteral(
        ":minecraft/%1/%2.json");

    QFileInfo finfo(filePathTemplate.arg(version, type));

    if (!finfo.exists() && (version != Game::defaultVersionString)) {
        qWarning() << "Game info file not exists:" << finfo.filePath()
                   << "; falling back to" << Game::defaultVersionString;
        finfo.setFile(filePathTemplate.arg(Game::defaultVersionString, type));
    }

    if (!finfo.exists()) {
        qWarning() << "Game info file not exists:" << finfo.filePath()
                   << "; falling back to" << Game::minimumVersionString;
        finfo.setFile(filePathTemplate.arg(Game::minimumVersionString, type));
    }

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "Game file not exists:" << finfo.filePath()
                   << "; return empty.";
        return {};
    }
    /*qDebug() << finfo; */
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError       errorPtr;
    const QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return {};
    }
    QJsonObject &&root = doc.object();
    if (root.isEmpty()) {
        /*qDebug() << "Root is empty. Return empty"; */
        return {};
    }

    if (root.contains(QLatin1String("base"))) {
        const auto &&tmpMap =
            loadInfo(type, root["base"].toString(), depth + 1);
        retMap.insert(std::move(tmpMap));
        root.remove(QLatin1String("base"));
    }
    if (root.contains(QLatin1String("removed"))) {
        const auto &&removed = root[QLatin1String("removed")].toArray();
        for (const auto &item: removed) {
            const QString &key = item.toString();
            if (retMap.contains(key))
                retMap.remove(key);
        }
        root.remove(QLatin1String("removed"));
    }
    if (root.contains(QLatin1String("added")))
        retMap.insert(root[QLatin1String("added")].toVariant().toMap());
    else
        retMap.insert(root.toVariantMap());

    return retMap;
}

using VanillaLookupMap = QMap<QString, QMap<QString, QString> >;

static VanillaLookupMap loadVainllaLookupMap() {
    QSet<QString>    versionPool;
    VanillaLookupMap res;

    QFile f(":/minecraft/" + Game::versionString() + "/vanilla_lookup.cbor");

    if (Q_LIKELY(f.exists())) {
        f.open(QIODevice::ReadOnly);
        const QByteArray &&data = f.readAll();
        f.close();

        QCborParserError   errorPtr;
        const QCborValue &&rootValue = QCborValue::fromCbor(data, &errorPtr);
        if (rootValue.isInvalid()) {
            qWarning() << "Parse failed" << errorPtr.errorString();
            return {};
        }
        const auto &root = rootValue.toMap();
        if (root.isEmpty()) {
            qWarning() << "Root is empty. Return empty";
            return {};
        }

        for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
            const auto            &aliases = it.value().toMap();
            QMap<QString, QString> aliasMap;
            for (auto it2 = aliases.cbegin(); it2 != aliases.cend(); ++it2) {
                aliasMap[it2.key().toString()] =
                    *versionPool.insert(it2.value().toString());
            }
            res[it.key().toString()] = std::move(aliasMap);
        }
    } else {
        QFile f2(":/minecraft/" + Game::versionString() +
                 "/vanilla_lookup.json");

        f2.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray &&data = f.readAll();
        f2.close();

        QJsonParseError       errorPtr;
        const QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
        if (doc.isNull()) {
            qWarning() << "Parse failed" << errorPtr.error;
            return {};
        }
        QJsonObject &&root = doc.object();
        if (root.isEmpty()) {
            qWarning() << "Root is empty. Return empty";
            return {};
        }

        for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
            const auto           &&value   = it.value();
            const auto            &aliases = value.toObject();
            QMap<QString, QString> aliasMap;
            for (auto it2 = aliases.constBegin(); it2 != aliases.constEnd();
                 ++it2) {
                aliasMap[it2.key()] =
                    *versionPool.insert(it2.value().toString());
                // aliasMap[it2.key()] = it2.value().toString();
            }
            res[it.key()] = std::move(aliasMap);
        }
    }

    return res;
}

static VanillaLookupMap &getVainllaLookupMap() {
    static VanillaLookupMap lookupTable = loadVainllaLookupMap();

    return lookupTable;
}

bool Game::isVanillaFileExists(const QString &catDir, const QString &path) {
    const static QString pathTemplate(
        QStringLiteral(":minecraft/%1/data-json/data/minecraft/%2/%3"));

    if (QFile::exists(pathTemplate.arg(Game::versionString(), catDir, path))) {
        return true;
    } else if (Game::version() > Game::v1_15) {
        const auto &lookupMap = getVainllaLookupMap();
        if (lookupMap.contains(catDir)) {
            const auto &&aliases = lookupMap.value(catDir);
            return aliases.contains(path);
        } else {
            return false;
        }
    } else {
        return false;
    }
}

QString Game::realVanillaFilePath(const QString &catDir, const QString &path) {
    const static QString pathTemplate(
        QStringLiteral(":minecraft/%1/data-json/data/minecraft/%2/%3"));

    const QString &&directPath = pathTemplate.arg(Game::versionString(),
                                                  catDir, path);

    if (QFile::exists(directPath)) {
        return directPath;
    } else if (Game::version() > Game::v1_15) {
        const auto &lookupMap = getVainllaLookupMap();
        if (lookupMap.contains(catDir)) {
            const auto &&aliases = lookupMap.value(catDir);
            if (aliases.contains(path)) {
                // qDebug() << catDir << path << aliases.value(path);
                return pathTemplate.arg(aliases.value(path), catDir, path);
            } else {
                return {};
            }
        } else {
            return {};
        }
    } else {
        return {};
    }
}
