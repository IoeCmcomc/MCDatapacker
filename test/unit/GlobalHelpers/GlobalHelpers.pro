QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase c++17

TEMPLATE = app

DEFINES += NO_GAME_H

SOURCES +=  tst_testglobalhelpers.cpp \
    ../../../src/codefile.cpp \
    ../../../src/globalhelpers.cpp

HEADERS += \
    ../../../src/codefile.h \
    ../../../src/globalhelpers.h

include($$PWD/../../../lib/uberswitch/uberswitch.pri)
