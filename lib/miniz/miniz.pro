# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TEMPLATE = lib

CONFIG += staticlib c++14

HEADERS = \
   $$PWD/miniz-cpp/zip.hpp

DISTFILES = \
   $$PWD/miniz-cpp/LICENSE.md

INCLUDEPATH += $$PWD/miniz-cpp
