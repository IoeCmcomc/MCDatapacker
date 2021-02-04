QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testliteralnode.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../../src/parsers/command/nodes/stringnode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../../src/parsers/command/nodes/stringnode.h
