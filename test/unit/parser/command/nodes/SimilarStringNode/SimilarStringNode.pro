QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testsimilarstringnode.cpp \
    ../../../../../../src/parsers/command/minecraftparser.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/boolnode.cpp \
    ../../../../../../src/parsers/command/nodes/doublenode.cpp \
    ../../../../../../src/parsers/command/nodes/floatnode.cpp \
    ../../../../../../src/parsers/command/nodes/integernode.cpp \
    ../../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../../src/parsers/command/nodes/similarstringnodes.cpp \
    ../../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../../src/parsers/command/parser.cpp

HEADERS += \
    ../../../../../../src/parsers/command/minecraftparser.h \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/boolnode.h \
    ../../../../../../src/parsers/command/nodes/doublenode.h \
    ../../../../../../src/parsers/command/nodes/floatnode.h \
    ../../../../../../src/parsers/command/nodes/integernode.h \
    ../../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../../src/parsers/command/nodes/similarstringnode.h \
    ../../../../../../src/parsers/command/nodes/similarstringnodes.h \
    ../../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../../src/parsers/command/nodes/stringnodes.h \
    ../../../../../../src/parsers/command/parser.h
