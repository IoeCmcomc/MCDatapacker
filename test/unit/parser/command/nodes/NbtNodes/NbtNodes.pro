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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/release/ -llib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../../../../lib/debug/ -llib
else:unix: LIBS += -L$$OUT_PWD/../../../../../../lib/ -llib

INCLUDEPATH += $$PWD/../../../../../../lib
DEPENDPATH += $$PWD/../../../../../../lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/release/liblib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/debug/liblib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/release/lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/debug/lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../../../lib/liblib.a
