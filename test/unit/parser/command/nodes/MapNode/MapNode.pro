QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testmapnode.cpp \
    ../../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h
