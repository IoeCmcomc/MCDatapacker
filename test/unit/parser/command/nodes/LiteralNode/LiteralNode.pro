QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testliteralnode.cpp \
    ../../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \

HEADERS += \
    ../../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
