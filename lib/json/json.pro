# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = json

TEMPLATE = lib

CONFIG += staticlib

HEADERS = \
   $$PWD/json/single_include/nlohmann/json.hpp

DISTFILES = \
   $$PWD/json/LICENSE.MIT

INCLUDEPATH += $$PWD/json/single_include

#DEFINES = 

