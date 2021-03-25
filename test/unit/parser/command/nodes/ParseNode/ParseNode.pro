QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testparsenode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/rootnode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/rootnode.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/release/ -llib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/debug/ -llib
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/ -llib

INCLUDEPATH += $$PWD/../../../../../../lib
DEPENDPATH += $$PWD/../../../../../../lib

INCLUDEPATH += $$PWD/../../../../../../lib/lru-cache/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/release/liblib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/debug/liblib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/release/lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/debug/lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/liblib.a
