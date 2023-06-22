# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = QFindDialogs

QT = core gui widgets

TEMPLATE = lib

CONFIG += staticlib

msvc:DEFINES += and="\"&&\"" or="\"||\""

HEADERS = \
   $$PWD/QFindDialogs/src/finddialog.h \
   $$PWD/QFindDialogs/src/findreplacedialog.h

SOURCES = \
   $$PWD/QFindDialogs/src/finddialog.cpp \
   $$PWD/QFindDialogs/src/findreplacedialog.cpp

DISTFILES = $$PWD/QFindDialogs/LICENSE

INCLUDEPATH += $$PWD/QFindDialogs/src

#DEFINES = 

