QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testparser.cpp \
    ../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../src/parsers/command/nodes/boolnode.cpp \
    ../../../../../src/parsers/command/nodes/doublenode.cpp \
    ../../../../../src/parsers/command/nodes/floatnode.cpp \
    ../../../../../src/parsers/command/nodes/integernode.cpp \
    ../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../src/parsers/command/parsenodecache.cpp \
    ../../../../../src/parsers/command/parser.cpp

HEADERS += \
    ../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../src/parsers/command/nodes/boolnode.h \
    ../../../../../src/parsers/command/nodes/doublenode.h \
    ../../../../../src/parsers/command/nodes/floatnode.h \
    ../../../../../src/parsers/command/nodes/integernode.h \
    ../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../src/parsers/command/parsenodecache.h \
    ../../../../../src/parsers/command/parser.h

RESOURCES += \
    ../../../../../resource/minecraft/info/1.15/1.15.qrc

DISTFILES += \
    ../../../../../resource/minecraft/info/1.15/mcdata/generated/reports/commands.json


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/release/ -llru-cache
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/debug/ -llru-cache
else:unix: LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/ -llru-cache

INCLUDEPATH += $$PWD/../../../../../lib/lru-cache \
    $$PWD/../../../../../lib/lru-cache/lru-cache/include

DEPENDPATH += $$PWD/../../../../../lib/lru-cache
