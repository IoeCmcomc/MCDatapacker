QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testnbtnodes.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/singlevaluenode.cpp \
    ../../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../../src/parsers/command/nodes/nbtnodes.cpp \
    ../../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += ../../../../../../src/parsers/command/nodes/argumentnode.h \
HEADERS += ../../../../../../src/parsers/command/nodes/singlevaluenode.h \
    ../../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../../src/parsers/command/nodes/nbtnodes.h \
    ../../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h
