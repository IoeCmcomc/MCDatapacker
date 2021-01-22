QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += --coverage -g -O0 -fprofile-arcs -ftest-coverage -fPIC
QMAKE_LFLAGS += --coverage -g -O0 -fprofile-arcs -ftest-coverage -fPIC

SOURCES +=  tst_testrootnode.cpp \
    ../../../../../../src/parsers/command/nodes/parsenode.cpp \
    ../../../../../../src/parsers/command/nodes/rootnode.cpp

HEADERS += \
    ../../../../../../src/parsers/command/nodes/parsenode.h \
    ../../../../../../src/parsers/command/nodes/rootnode.h
