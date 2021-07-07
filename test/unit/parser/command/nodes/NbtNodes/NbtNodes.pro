QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testnbtnodes.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/mapnode.cpp \
    ../../../../../../src/parsers/command/nodes/nbtnodes.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/mapnode.h \
    ../../../../../../src/parsers/command/nodes/nbtnodes.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/release/ -lnbt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/debug/ -lnbt
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/nbt/ -lnbt

INCLUDEPATH += $$PWD/../../../../../../lib/nbt \
    $$PWD/../../../../../../lib/nbt/nbt-cpp/include
DEPENDPATH += $$PWD/../../../../../../lib/nbt
