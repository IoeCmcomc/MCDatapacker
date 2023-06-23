QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testschemaparser.cpp \
    ../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../src/parsers/command/nodes/singlevaluenode.cpp \
    ../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../src/parsers/command/parsenodecache.cpp \
    ../../../../../src/parsers/command/schemaparser.cpp \
    ../../../../../src/parsers/command/schema/schemaargumentnode.cpp \
    ../../../../../src/parsers/command/schema/schemaliteralnode.cpp \
    ../../../../../src/parsers/command/schema/schemanode.cpp \
    ../../../../../src/parsers/command/schema/schemarootnode.cpp \
    ../../../../../src/parsers/parser.cpp \
    ../../../../../src/parsers/command/re2c_generated_functions.cpp

HEADERS += \
    ../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../src/parsers/command/nodes/singlevaluenode.h \
    ../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../src/parsers/command/parsenodecache.h \
    ../../../../../src/parsers/command/schemaparser.h \
    ../../../../../src/parsers/command/schema/schemaargumentnode.h \
    ../../../../../src/parsers/command/schema/schemaliteralnode.h \
    ../../../../../src/parsers/command/schema/schemanode.h \
    ../../../../../src/parsers/command/schema/schemarootnode.h \
    ../../../../../src/parsers/parser.h \
    ../../../../../src/parsers/command/re2c_generated_functions.h

RESOURCES += \
    ../../../../../resource/minecraft/info/1.15/1.15.qrc

DISTFILES += \
    ../../../../../resource/minecraft/info/1.15/summary/commands/data.min.json \
    ../../../../../resource/minecraft/info/1.15/summary/commands/data.msgpack

include($$PWD/../../../../../lib/lru-cache/lru-cache.pri)
include($$PWD/../../../../../lib/json/json.pri)
include($$PWD/../../../../../lib/uberswitch/uberswitch.pri)
