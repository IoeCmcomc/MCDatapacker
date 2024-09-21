#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

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
    inline const QVersionNumber v1_20_4{ 1, 20, 4 };
    inline const QVersionNumber v1_20_6{ 1, 20, 6 };
    inline const QVersionNumber v1_21{ 1, 21 };
    inline const QVersionNumber v1_21_2{ 1, 21, 2 };
    inline const QVersionNumber v1_22{ 1, 22 };

    inline const auto *minimumVersionString = "1.15";
    inline const auto *defaultVersionString = "1.20.6";
}

#endif // GAMECONSTANTS_H
