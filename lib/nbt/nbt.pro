# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = nbt

TEMPLATE = lib

CONFIG += c++17 staticlib

HEADERS = \
   $$PWD/nbt-cpp/include/nbt.hpp \
   $$PWD/nbt-cpp/src/nbt_internal.hpp

SOURCES = \
   $$PWD/nbt-cpp/src/nbt.cpp

DISTFILES = $$PWD/nbt-cpp/LICENSE

INCLUDEPATH += $$PWD/nbt-cpp/include

#DEFINES = 

