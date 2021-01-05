QT       += core gui uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 lrelease embed_translations

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
    aboutdialog.cpp \
    basecondition.cpp \
    blockitemselectordialog.cpp \
    codeeditor.cpp \
    codefile.cpp \
    datapackfileiconprovider.cpp \
    datapacktreeview.cpp \
    dialogdatabutton.cpp \
    entityconditiondialog.cpp \
    extendeddelegate.cpp \
    fileswitcher.cpp \
    globalhelpers.cpp \
    highlighter.cpp \
    itemconditiondialog.cpp \
    jsonhighlighter.cpp \
    locationconditiondialog.cpp \
    loottableeditordock.cpp \
    loottableentry.cpp \
    loottablefunction.cpp \
    main.cpp \
    mainwindow.cpp \
    mcfunctionhighlighter.cpp \
    mcrinvitem.cpp \
    mcrinvslot.cpp \
    mcrinvsloteditor.cpp \
    mcritemsortfilterproxymodel.cpp \
    mcrpredcondition.cpp \
    modelfunctions.cpp \
    newdatapackdialog.cpp \
    numericinput.cpp \
    optionalspinbox.cpp \
    predicatedock.cpp \
    rawjsontextedit.cpp \
    settingsdialog.cpp \
    stackedwidget.cpp \
    tabbedcodeeditorinterface.cpp \
    tagselectordialog.cpp \
    truefalsebox.cpp \
    vieweventfilter.cpp \
    visualrecipeeditordock.cpp

HEADERS += \
    aboutdialog.h \
    basecondition.h \
    blockitemselectordialog.h \
    codeeditor.h \
    codefile.h \
    datapackfileiconprovider.h \
    datapacktreeview.h \
    dialogdatabutton.h \
    entityconditiondialog.h \
    extendeddelegate.h \
    fileswitcher.h \
    globalhelpers.h \
    highlighter.h \
    itemconditiondialog.h \
    jsonhighlighter.h \
    linenumberarea.h \
    locationconditiondialog.h \
    loottableeditordock.h \
    loottableentry.h \
    loottablefunction.h \
    mainwindow.h \
    mcfunctionhighlighter.h \
    mcrinvitem.h \
    mcrinvslot.h \
    mcrinvsloteditor.h \
    mcritemsortfilterproxymodel.h \
    mcrpredcondition.h \
    modelfunctions.h \
    newdatapackdialog.h \
    numericinput.h \
    optionalspinbox.h \
    predicatedock.h \
    rawjsontextedit.h \
    settingsdialog.h \
    stackedwidget.h \
    tabbedcodeeditorinterface.h \
    tagselectordialog.h \
    truefalsebox.h \
    vieweventfilter.h \
    visualrecipeeditordock.h

FORMS += \
    aboutdialog.ui \
    blockitemselectordialog.ui \
    dialogdatabutton.ui \
    entityconditiondialog.ui \
    itemconditiondialog.ui \
    locationconditiondialog.ui \
    loottableeditordock.ui \
    loottableentry.ui \
    loottablefunction.ui \
    mainwindow.ui \
    mcrinvsloteditor.ui \
    mcrpredcondition.ui \
    newdatapackdialog.ui \
    numericinput.ui \
    predicatedock.ui \
    rawjsontextedit.ui \
    settingsdialog.ui \
    tabbedcodeeditorinterface.ui \
    tagselectordialog.ui \
    visualrecipeeditordock.ui

TRANSLATIONS += \
    MCDatapacker_vi_VN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/app/app.qrc \
    resource/minecraft/minecraft.qrc

RC_ICONS = resource/app/icon/favicon.ico

VERSION = 0.3.0
QMAKE_TARGET_COMPANY = IoeCmcomc
QMAKE_TARGET = MCDatapacker
QMAKE_TARGET_PRODUCT = MCDatapacker
