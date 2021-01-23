QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testdoublenode.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/doublenode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/doublenode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h
