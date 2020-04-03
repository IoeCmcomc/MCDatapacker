QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blockitemselectordialog.cpp \
    codeeditor.cpp \
    datapacktreeview.cpp \
    extendednumericdelegate.cpp \
    extendednumericinput.cpp \
    globalhelpers.cpp \
    itemconditiondialog.cpp \
    jsonhighlighter.cpp \
    loottableeditordock.cpp \
    main.cpp \
    mainwindow.cpp \
    mcfunctionhighlighter.cpp \
    mcrinvitem.cpp \
    mcrinvslot.cpp \
    mcrinvsloteditor.cpp \
    mcritemsortfilterproxymodel.cpp \
    mcrpredcondition.cpp \
    newdatapackdialog.cpp \
    predicatedock.cpp \
    settingsdialog.cpp \
    stackedwidget.cpp \
    tagselectordialog.cpp \
    visualrecipeeditordock.cpp

HEADERS += \
    blockitemselectordialog.h \
    codeeditor.h \
    datapacktreeview.h \
    extendednumericdelegate.h \
    extendednumericinput.h \
    globalhelpers.h \
    itemconditiondialog.h \
    jsonhighlighter.h \
    linenumberarea.h \
    loottableeditordock.h \
    mainwindow.h \
    mcfunctionhighlighter.h \
    mcrinvitem.h \
    mcrinvslot.h \
    mcrinvsloteditor.h \
    mcritemsortfilterproxymodel.h \
    mcrpredcondition.h \
    newdatapackdialog.h \
    predicatedock.h \
    settingsdialog.h \
    stackedwidget.h \
    tagselectordialog.h \
    visualrecipeeditordock.h

FORMS += \
    blockitemselectordialog.ui \
    extendednumericinput.ui \
    itemconditiondialog.ui \
    loottableeditordock.ui \
    mainwindow.ui \
    mcrinvsloteditor.ui \
    mcrpredcondition.ui \
    newdatapackdialog.ui \
    predicatedock.ui \
    settingsdialog.ui \
    tagselectordialog.ui \
    visualrecipeeditordock.ui

TRANSLATIONS += \
    MCDatapacker_vi_VN.ts

QT += uitools

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/minecraft/minecraft.qrc
