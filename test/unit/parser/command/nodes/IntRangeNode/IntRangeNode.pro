QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testintrangenode.cpp \
    ../../../../../../src/parsers/command/nodes/argumentnode.cpp \
    ../../../../../../src/parsers/command/nodes/integernode.cpp \
    ../../../../../../src/parsers/command/nodes/intrangenode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/argumentnode.h \
    ../../../../../../src/parsers/command/nodes/integernode.h \
    ../../../../../../src/parsers/command/nodes/intrangenode.h \
    ../../../../../../src/parsers/command/nodes/parsenode.h
