QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testsimilarresourcelocationnodes.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/resourcelocationnode.cpp \
    ../../../../../../src/parsers/command/nodes/similarresourcelocationnodes.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/resourcelocationnode.h \
    ../../../../../../src/parsers/command/nodes/similarresourcelocationnodes.h
