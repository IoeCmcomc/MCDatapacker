#ifndef GAME_H
#define GAME_H

#include "gameconstants.h"

using VanillaLookupMap = QMap<QString, QMap<QString, QString> >;

namespace Game {
    QVersionNumber version();
    QString versionString();

    QVariantMap getInfo(const QString &type);
    QVariantMap getInfo(const QString &type, const QString &version);
    QVariantMap loadInfo(const QString &type, const QString &version,
                         const int depth = 0);
    QVector<QString> getRegistry(const QString &type);
    QVector<QString> getRegistry(const QString &type, const QString &version);
    QVector<QString> loadRegistry(const QString &type, const QString &version);

    VanillaLookupMap getVainllaLookupMap();
    bool isVanillaFileExists(const QString &catDir, const QString &path);
    QString realVanillaFilePath(const QString &catDir, const QString &path);

    QString canonicalCategory(QStringView catDir);
    QString canonicalCategory(QStringView catDir,
                              const QVersionNumber &version);
}

#define FROM_VER(ver)     (Game::version() >= Game::ver)
#define UNTIL_VER(ver)    (Game::version() < Game::ver)

#endif // GAME_H
