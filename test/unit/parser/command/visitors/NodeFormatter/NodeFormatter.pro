QT += testlib gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_textnodeformatter.cpp \
    ../../../../../../src/parsers/command/minecraftparser.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/axesnode.cpp \
    ../../../../../../src/parsers/command/nodes/axisnode.cpp \
    ../../../../../../src/parsers/command/nodes/blockstatenode.cpp \
    ../../../../../../src/parsers/command/nodes/boolnode.cpp \
    ../../../../../../src/parsers/command/nodes/componentnode.cpp \
    ../../../../../../src/parsers/command/nodes/doublenode.cpp \
    ../../../../../../src/parsers/command/nodes/entitynode.cpp \
    ../../../../../../src/parsers/command/nodes/floatnode.cpp \
    ../../../../../../src/parsers/command/nodes/floatrangenode.cpp \
    ../../../../../../src/parsers/command/nodes/integernode.cpp \
    ../../../../../../src/parsers/command/nodes/intrangenode.cpp \
    ../../../../../../src/parsers/command/nodes/itemstacknode.cpp \
    ../../../../../../src/parsers/command/nodes/literalnode.cpp \
    ../../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../../src/parsers/command/nodes/multimapnode.cpp \
    ../../../../../../src/parsers/command/nodes/nbtnodes.cpp \
    ../../../../../../src/parsers/command/nodes/nbtpathnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/particlenode.cpp \
    ../../../../../../src/parsers/command/nodes/resourcelocationnode.cpp \
    ../../../../../../src/parsers/command/nodes/rootnode.cpp \
    ../../../../../../src/parsers/command/nodes/similaraxesnodes.cpp \
    ../../../../../../src/parsers/command/nodes/similarresourcelocationnodes.cpp \
    ../../../../../../src/parsers/command/nodes/similarstringnodes.cpp \
    ../../../../../../src/parsers/command/nodes/stringnode.cpp \
    ../../../../../../src/parsers/command/nodes/swizzlenode.cpp \
    ../../../../../../src/parsers/command/nodes/targetselectornode.cpp \
    ../../../../../../src/parsers/command/nodes/timenode.cpp \
    ../../../../../../src/parsers/command/nodes/uuidnode.cpp \
    ../../../../../../src/parsers/command/parsenodecache.cpp \
    ../../../../../../src/parsers/command/parser.cpp \
    ../../../../../../src/parsers/command/visitors/nodeformatter.cpp \
    ../../../../../../src/parsers/command/visitors/nodevisitor.cpp

HEADERS += \
    ../../../../../../src/parsers/command/minecraftparser.h \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/axesnode.h \
    ../../../../../../src/parsers/command/nodes/axisnode.h \
    ../../../../../../src/parsers/command/nodes/blockstatenode.h \
    ../../../../../../src/parsers/command/nodes/boolnode.h \
    ../../../../../../src/parsers/command/nodes/componentnode.h \
    ../../../../../../src/parsers/command/nodes/doublenode.h \
    ../../../../../../src/parsers/command/nodes/entitynode.h \
    ../../../../../../src/parsers/command/nodes/floatnode.h \
    ../../../../../../src/parsers/command/nodes/floatrangenode.h \
    ../../../../../../src/parsers/command/nodes/integernode.h \
    ../../../../../../src/parsers/command/nodes/intrangenode.h \
    ../../../../../../src/parsers/command/nodes/itemstacknode.h \
    ../../../../../../src/parsers/command/nodes/literalnode.h \
    ../../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../../src/parsers/command/nodes/multimapnode.h \
    ../../../../../../src/parsers/command/nodes/nbtnodes.h \
    ../../../../../../src/parsers/command/nodes/nbtpathnode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/particlenode.h \
    ../../../../../../src/parsers/command/nodes/rangenode.h \
    ../../../../../../src/parsers/command/nodes/resourcelocationnode.h \
    ../../../../../../src/parsers/command/nodes/rootnode.h \
    ../../../../../../src/parsers/command/nodes/similaraxesnodes.h \
    ../../../../../../src/parsers/command/nodes/similarresourcelocationnodes.h \
    ../../../../../../src/parsers/command/nodes/similarstringnodes.h \
    ../../../../../../src/parsers/command/nodes/stringnode.h \
    ../../../../../../src/parsers/command/nodes/swizzlenode.h \
    ../../../../../../src/parsers/command/nodes/targetselectornode.h \
    ../../../../../../src/parsers/command/nodes/timenode.h \
    ../../../../../../src/parsers/command/nodes/uuidnode.h \
    ../../../../../../src/parsers/command/parsenodecache.h \
    ../../../../../../src/parsers/command/parser.h \
    ../../../../../../src/parsers/command/visitors/nodeformatter.h \
    ../../../../../../src/parsers/command/visitors/nodevisitor.h

RESOURCES += \
    ../../../../../../resource/minecraft/info/1.15/1.15.qrc

DISTFILES += \
    ../../../../../../resource/minecraft/info/1.15/mcdata/generated/reports/commands.json

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/lru-cache/release/ -llru-cache
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/lru-cache/debug/ -llru-cache
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/lru-cache/ -llru-cache

INCLUDEPATH += $$PWD/../../../../../../lib/lru-cache \
    $$PWD/../../../../../../lib/lru-cache/lru-cache/include
DEPENDPATH += $$PWD/../../../../../../lib/lru-cache


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/release/ -lnbt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/debug/ -lnbt
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/ -lnbt

INCLUDEPATH += $$PWD/../../../../../../lib/nbt \
    $$PWD/../../../../../../lib/nbt/nbt-cpp/include
DEPENDPATH += $$PWD/../../../../../../lib/nbt


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/json/release/ -ljson
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/json/debug/ -ljson
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/json/ -ljson

INCLUDEPATH += $$PWD/../../../../../../lib/json \
    $$PWD/../../../../../../lib/json/json/single_include
DEPENDPATH += $$PWD/../../../../../../lib/json
