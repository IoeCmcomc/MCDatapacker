QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testminecraftparser.cpp \
    ../../../../../src/parsers/command/minecraftparser.cpp \
    ../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../src/parsers/command/nodes/axesnode.cpp \
    ../../../../../src/parsers/command/nodes/axisnode.cpp \
    ../../../../../src/parsers/command/nodes/boolnode.cpp \
    ../../../../../src/parsers/command/nodes/doublenode.cpp \
    ../../../../../src/parsers/command/nodes/floatnode.cpp \
    ../../../../../src/parsers/command/nodes/floatrangenode.cpp \
    ../../../../../src/parsers/command/nodes/integernode.cpp \
    ../../../../../src/parsers/command/nodes/intrangenode.cpp \
    ../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../src/parsers/command/nodes/resourcelocationnode.cpp \
    ../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../src/parsers/command/nodes/similaraxesnodes.cpp \
    ../../../../../src/parsers/command/nodes/similarresourcelocationnodes.cpp \
    ../../../../../src/parsers/command/nodes/similarstringnodes.cpp \
    ../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../src/parsers/command/nodes/swizzlenode.cpp \
    ../../../../../src/parsers/command/nodes/timenode.cpp \
    ../../../../../src/parsers/command/nodes/uuidnode.cpp \
    ../../../../../src/parsers/command/parser.cpp

HEADERS += \
    ../../../../../src/parsers/command/minecraftparser.h \
    ../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../src/parsers/command/nodes/axesnode.h \
    ../../../../../src/parsers/command/nodes/axisnode.h \
    ../../../../../src/parsers/command/nodes/boolnode.h \
    ../../../../../src/parsers/command/nodes/doublenode.h \
    ../../../../../src/parsers/command/nodes/floatnode.h \
    ../../../../../src/parsers/command/nodes/floatrangenode.h \
    ../../../../../src/parsers/command/nodes/integernode.h \
    ../../../../../src/parsers/command/nodes/intrangenode.h \
    ../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../src/parsers/command/nodes/rangenode.h \
    ../../../../../src/parsers/command/nodes/resourcelocationnode.h \
    ../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../src/parsers/command/nodes/similaraxesnodes.h \
    ../../../../../src/parsers/command/nodes/similarresourcelocationnodes.h \
    ../../../../../src/parsers/command/nodes/similarstringnodes.h \
    ../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../src/parsers/command/nodes/swizzlenode.h \
    ../../../../../src/parsers/command/nodes/timenode.h \
    ../../../../../src/parsers/command/nodes/uuidnode.h \
    ../../../../../src/parsers/command/parser.h

RESOURCES += \
    ../../../../../resource/minecraft/minecraft.qrc
