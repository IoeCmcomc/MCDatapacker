QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testtimenode.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/timenode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/timenode.h
