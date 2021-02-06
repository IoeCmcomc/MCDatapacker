QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testaxisnode.cpp \
    ../../../../../../src/parsers/command/nodes/axisnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/axisnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h
