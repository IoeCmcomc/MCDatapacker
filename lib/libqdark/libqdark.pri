include(libqdark/defaults.pri)

HEADERS += \
    $$PWD/libqdark/src/SystemThemeHelper.h

SOURCES += \
    $$PWD/libqdark/src/SystemThemeHelper.cpp

win32 {
    HEADERS += \
        $$PWD/libqdark/src/windows10themenotifier.h

    SOURCES += \
        $$PWD/libqdark/src/windows10themenotifier.cpp
}
