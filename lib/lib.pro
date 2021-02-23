TEMPLATE = lib

CONFIG += staticlib qt gui c++17 conan_basic_setup

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DISTFILES += \
    QFindDialogs/LICENSE \
    QFindDialogs/src/README.txt

HEADERS += \
    QFindDialogs/src/finddialog.h \
    QFindDialogs/src/findreplacedialog.h \
    json/single_include/nlohmann/json.hpp \
    nbt-cpp/include/nbt.hpp \
    nbt-cpp/src/nbt_internal.hpp

SOURCES += \
    QFindDialogs/src/finddialog.cpp \
    QFindDialogs/src/findreplacedialog.cpp \
    nbt-cpp/src/nbt.cpp
