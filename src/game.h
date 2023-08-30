#ifndef GAME_H
#define GAME_H

#include <QVersionNumber>

namespace Game {
    inline const QVersionNumber v1_15{ 1, 15 };
    inline const QVersionNumber v1_16{ 1, 16 };
    inline const QVersionNumber v1_17{ 1, 17 };
    inline const QVersionNumber v1_18{ 1, 18 };
    inline const QVersionNumber v1_18_2{ 1, 18, 2 };
    inline const QVersionNumber v1_19{ 1, 19 };
    inline const QVersionNumber v1_19_3{ 1, 19, 3 };
    inline const QVersionNumber v1_19_4{ 1, 19, 4 };
    inline const QVersionNumber v1_20{ 1, 20 };
    inline const QVersionNumber v1_20_2{ 1, 20, 2 };

    inline const auto minimumVersionString = "1.15";
    inline const auto defaultVersionString = "1.20";

    QVersionNumber version();
    QString versionString();

    QVariantMap getInfo(const QString &type);
    QVariantMap getInfo(const QString &type, const QString &version,
                        const int depth = 0);
    QVariantMap loadInfo(const QString &type, const QString &version,
                         const int depth = 0);
    QVector<QString> getRegistry(const QString &type);
    QVector<QString> getRegistry(const QString &type, const QString &version);
    QVector<QString> loadRegistry(const QString &type, const QString &version);
}

#endif // GAME_H
