#include "game.h"

#include "lru/lru.hpp"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QVersionNumber Game::version() {
    return QVersionNumber::fromString(versionString());
}

QString Game::versionString() {
    return QSettings().value(QStringLiteral("general/gameVersion"), defaultVersionString).toString();
}

QStringList Game::getRegistry(const QString &type) {
    return getRegistry(type, Game::versionString());
}

QStringList Game::getRegistry(const QString &type, const QString &version) {
    static LRU::Cache<std::tuple<QString, QString>, QStringList> cache{20};
    const std::tuple inputPair{type, version};
    if (cache.contains(inputPair)) return cache.lookup(inputPair);

    const auto &&result = loadRegistry(type, version);

    cache.emplace(std::piecewise_construct,
                    std::forward_as_tuple(type, version),
                    std::forward_as_tuple(result));
    return result;
}

QStringList Game::loadRegistry(const QString &type, const QString &version) {
    QFileInfo finfo(":minecraft/" + version + "/registries/" + type + "/data.min.json");

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.path() << "Return empty.";
        return {};
    }
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
    const QJsonDocument &&doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        qWarning() << "Parse failed" << errorPtr.error;
        return {};
    }

    QStringList values;
    const auto &&array = doc.array();
    for (const auto &value : array) {
        values << value.toString();
    }
    return values;
}

QVariantMap Game::getInfo(const QString &type) {
    return getInfo(type, Game::versionString());
}

QVariantMap Game::getInfo(const QString &type, const QString &version, const int depth)
{
    static LRU::Cache<std::tuple<QString, QString>, QVariantMap> cache{20};
    const std::tuple inputPair{type, version};
    if (cache.contains(inputPair)) return cache.lookup(inputPair);

    const auto &&result = loadInfo(type, version, depth);

    cache.emplace(std::piecewise_construct,
                    std::forward_as_tuple(type, version),
                    std::forward_as_tuple(result));
    return result;
}

QVariantMap Game::loadInfo(const QString &type, const QString &version, const int depth) {
    QVariantMap retMap;

    QFileInfo finfo(":minecraft/" + version + "/" + type + ".json");

    //qDebug() << type << version << finfo << finfo.exists() << depth;

    if (!finfo.exists())
        finfo.setFile(QStringLiteral(":minecraft/1.15/") + type + ".json");

    if (!(finfo.exists() && finfo.isFile())) {
        qWarning() << "File not exists:" << finfo.path() << "Return empty.";
        return {};
    }
    /*qDebug() << finfo; */
    QFile inFile(finfo.filePath());
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray &&data = inFile.readAll();
    inFile.close();

    QJsonParseError errorPtr;
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
        const auto &&tmpMap = getInfo(type, root["base"].toString(), depth+1);
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