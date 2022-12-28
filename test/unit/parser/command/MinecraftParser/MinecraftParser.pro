QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

win32 {
QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage -lgcov
QMAKE_LFLAGS_WINDOWS += -g -fprofile-arcs -ftest-coverage -lgcov
}

SOURCES +=  tst_testminecraftparser.cpp \
    ../../../../../src/parsers/command/minecraftparser.cpp \
    ../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../src/parsers/command/nodes/axesnode.cpp \
    ../../../../../src/parsers/command/nodes/anglenode.cpp \
    ../../../../../src/parsers/command/nodes/blockstatenode.cpp \
    ../../../../../src/parsers/command/nodes/componentnode.cpp \
    ../../../../../src/parsers/command/nodes/entitynode.cpp \
    ../../../../../src/parsers/command/nodes/singlevaluenode.cpp \
    ../../../../../src/parsers/command/nodes/floatrangenode.cpp \
    ../../../../../src/parsers/command/nodes/intrangenode.cpp \
    ../../../../../src/parsers/command/nodes/itemstacknode.cpp \
    ../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../src/parsers/command/nodes/nbtnodes.cpp \
    ../../../../../src/parsers/command/nodes/nbtpathnode.cpp \
    ../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../src/parsers/command/nodes/particlenode.cpp \
    ../../../../../src/parsers/command/nodes/resourcelocationnode.cpp \
    ../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../src/parsers/command/nodes/swizzlenode.cpp \
    ../../../../../src/parsers/command/nodes/targetselectornode.cpp \
    ../../../../../src/parsers/command/nodes/timenode.cpp \
    ../../../../../src/parsers/command/parsenodecache.cpp \
    ../../../../../src/parsers/command/parser.cpp \
    ../../../../../src/parsers/command/schema/schemaargumentnode.cpp \
    ../../../../../src/parsers/command/schema/schemaliteralnode.cpp \
    ../../../../../src/parsers/command/schema/schemanode.cpp \
    ../../../../../src/parsers/command/schema/schemarootnode.cpp \
    ../../../../../src/parsers/command/visitors/reprprinter.cpp

HEADERS += \
    ../../../../../src/parsers/command/minecraftparser.h \
    ../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../src/parsers/command/nodes/axesnode.h \
    ../../../../../src/parsers/command/nodes/anglenode.h \
    ../../../../../src/parsers/command/nodes/blockstatenode.h \
    ../../../../../src/parsers/command/nodes/componentnode.h \
    ../../../../../src/parsers/command/nodes/entitynode.h \
    ../../../../../src/parsers/command/nodes/singlevaluenode.h \
    ../../../../../src/parsers/command/nodes/floatrangenode.h \
    ../../../../../src/parsers/command/nodes/intrangenode.h \
    ../../../../../src/parsers/command/nodes/itemstacknode.h \
    ../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../src/parsers/command/nodes/nbtnodes.h \
    ../../../../../src/parsers/command/nodes/nbtpathnode.h \
    ../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../src/parsers/command/nodes/particlenode.h \
    ../../../../../src/parsers/command/nodes/rangenode.h \
    ../../../../../src/parsers/command/nodes/resourcelocationnode.h \
    ../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../src/parsers/command/nodes/swizzlenode.h \
    ../../../../../src/parsers/command/nodes/targetselectornode.h \
    ../../../../../src/parsers/command/nodes/timenode.h \
    ../../../../../src/parsers/command/parsenodecache.h \
    ../../../../../src/parsers/command/parser.h \
    ../../../../../src/parsers/command/schema/schemaargumentnode.h \
    ../../../../../src/parsers/command/schema/schemaliteralnode.h \
    ../../../../../src/parsers/command/schema/schemanode.h \
    ../../../../../src/parsers/command/schema/schemarootnode.h \
    ../../../../../src/parsers/command/visitors/reprprinter.cpp

RESOURCES += \
    ../../../../../resource/minecraft/info/1.18.2/1.18.2.qrc

DISTFILES += \
    ../../../../../resource/minecraft/info/1.18.2/summary/commands/data.min.json

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/release/ -llru-cache
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/debug/ -llru-cache
else:unix: LIBS += -L$$OUT_PWD/../../../../../lib/lru-cache/ -llru-cache

INCLUDEPATH += $$PWD/../../../../../lib/lru-cache \
    $$PWD/../../../../../lib/lru-cache/lru-cache/include
DEPENDPATH += $$PWD/../../../../../lib/lru-cache

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/lru-cache/release/liblru-cache.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/lru-cache/debug/liblru-cache.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/lru-cache/release/lru-cache.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/lru-cache/debug/lru-cache.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/lru-cache/liblru-cache.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/nbt/release/ -lnbt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/nbt/debug/ -lnbt
else:unix: LIBS += -L$$OUT_PWD/../../../../../lib/nbt/ -lnbt

INCLUDEPATH += $$PWD/../../../../../lib/nbt \
    $$PWD/../../../../../lib/nbt/nbt-cpp/include
DEPENDPATH += $$PWD/../../../../../lib/nbt

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/nbt/release/libnbt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/nbt/debug/libnbt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/nbt/release/nbt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/nbt/debug/nbt.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/nbt/libnbt.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/json/release/ -ljson
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../lib/json/debug/ -ljson
else:unix: LIBS += -L$$OUT_PWD/../../../../../lib/json/ -ljson

INCLUDEPATH += $$PWD/../../../../../lib/json \
    $$PWD/../../../../../lib/json/json/single_include
DEPENDPATH += $$PWD/../../../../../lib/json

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/json/release/libjson.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/json/debug/libjson.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/json/release/json.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/json/debug/json.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../lib/json/libjson.a
