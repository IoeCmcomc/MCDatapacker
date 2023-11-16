QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    ../../../../src/parsers/linesplitter.cpp \
    tst_testlinesplitter.cpp

HEADERS += \
    ../../../../src/parsers/linesplitter.h
