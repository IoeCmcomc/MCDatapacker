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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/release/ -llib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/debug/ -llib
else:unix: LIBS += -L$$OUT_PWD/../../../../../lib/ -llib

INCLUDEPATH += $$PWD/../../../../../lib
DEPENDPATH += $$PWD/../../../../../lib

INCLUDEPATH += $$PWD/../../../../../lib/lru-cache/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/release/liblib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/debug/liblib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/release/lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/debug/lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/liblib.a

DISTFILES += \
    ../../../../../resource/minecraft/info/1.15/mcdata/generated/reports/commands.json
