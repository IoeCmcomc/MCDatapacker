CONFIG      += plugin debug_and_release c++17
TARGET      = $$qtLibraryTarget(mcdatapackerwidgetsplugin)
TEMPLATE    = lib

DEFINES += "__clang_major__=11"

HEADERS     = \
    extendedtablewidget/extendedtablewidgetplugin.h \
    extendedtablewidget/extendedtablewidgetcontainerextension.h \
    extendedtablewidget/extendedtablewidgetextensionfactory.h \
    numberprovider/numberproviderplugin.h \
    dialogdatabutton/dialogdatabuttonplugin.h \
    multipagewidget/multipagewidgetplugin.h \
    multipagewidget/multipagewidgetcontainerextension.h \
    multipagewidget/multipagewidgetextensionfactory.h \
    mcdatapackerwidgets.h
SOURCES     = \
    extendedtablewidget/extendedtablewidgetplugin.cpp \
    extendedtablewidget/extendedtablewidgetcontainerextension.cpp \
    extendedtablewidget/extendedtablewidgetextensionfactory.cpp \
    numberprovider/numberproviderplugin.cpp \
    dialogdatabutton/dialogdatabuttonplugin.cpp \
    multipagewidget/multipagewidgetplugin.cpp \
    multipagewidget/multipagewidgetcontainerextension.cpp \
    multipagewidget/multipagewidgetextensionfactory.cpp \
    mcdatapackerwidgets.cpp

LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(../mcdatapackerwidgets.pri)
