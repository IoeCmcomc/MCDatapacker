TEMPLATE = lib

CONFIG += staticlib qt gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DISTFILES += \
    QFindDialogs/LICENSE \
    QFindDialogs/src/README.txt

HEADERS += \
    QFindDialogs/src/finddialog.h \
    QFindDialogs/src/findreplacedialog.h

SOURCES += \
    QFindDialogs/src/finddialog.cpp \
    QFindDialogs/src/findreplacedialog.cpp
