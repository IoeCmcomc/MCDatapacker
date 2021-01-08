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
    src/aboutdialog.cpp \
    src/basecondition.cpp \
    src/blockitemselectordialog.cpp \
    src/codeeditor.cpp \
    src/codefile.cpp \
    src/datapackfileiconprovider.cpp \
    src/datapacktreeview.cpp \
    src/dialogdatabutton.cpp \
    src/disclaimerdialog.cpp \
    src/entityconditiondialog.cpp \
    src/extendeddelegate.cpp \
    src/fileswitcher.cpp \
    src/globalhelpers.cpp \
    src/highlighter.cpp \
    src/itemconditiondialog.cpp \
    src/jsonhighlighter.cpp \
    src/locationconditiondialog.cpp \
    src/loottableeditordock.cpp \
    src/loottableentry.cpp \
    src/loottablefunction.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mcfunctionhighlighter.cpp \
    src/mcrinvitem.cpp \
    src/mcrinvslot.cpp \
    src/mcrinvsloteditor.cpp \
    src/mcritemsortfilterproxymodel.cpp \
    src/mcrpredcondition.cpp \
    src/modelfunctions.cpp \
    src/newdatapackdialog.cpp \
    src/numericinput.cpp \
    src/optionalspinbox.cpp \
    src/predicatedock.cpp \
    src/rawjsontextedit.cpp \
    src/settingsdialog.cpp \
    src/stackedwidget.cpp \
    src/tabbedcodeeditorinterface.cpp \
    src/tagselectordialog.cpp \
    src/truefalsebox.cpp \
    src/vieweventfilter.cpp \
    src/visualrecipeeditordock.cpp

HEADERS += \
    src/aboutdialog.h \
    src/basecondition.h \
    src/blockitemselectordialog.h \
    src/codeeditor.h \
    src/codefile.h \
    src/datapackfileiconprovider.h \
    src/datapacktreeview.h \
    src/dialogdatabutton.h \
    src/disclaimerdialog.h \
    src/entityconditiondialog.h \
    src/extendeddelegate.h \
    src/fileswitcher.h \
    src/globalhelpers.h \
    src/highlighter.h \
    src/itemconditiondialog.h \
    src/jsonhighlighter.h \
    src/linenumberarea.h \
    src/locationconditiondialog.h \
    src/loottableeditordock.h \
    src/loottableentry.h \
    src/loottablefunction.h \
    src/mainwindow.h \
    src/mcfunctionhighlighter.h \
    src/mcrinvitem.h \
    src/mcrinvslot.h \
    src/mcrinvsloteditor.h \
    src/mcritemsortfilterproxymodel.h \
    src/mcrpredcondition.h \
    src/modelfunctions.h \
    src/newdatapackdialog.h \
    src/numericinput.cpp.GANRTu \
    src/numericinput.h \
    src/optionalspinbox.h \
    src/predicatedock.h \
    src/rawjsontextedit.h \
    src/settingsdialog.h \
    src/stackedwidget.h \
    src/tabbedcodeeditorinterface.h \
    src/tagselectordialog.h \
    src/truefalsebox.h \
    src/vieweventfilter.h \
    src/visualrecipeeditordock.h

FORMS += \
    src/aboutdialog.ui \
    src/blockitemselectordialog.ui \
    src/dialogdatabutton.ui \
    src/disclaimerdialog.ui \
    src/entityconditiondialog.ui \
    src/itemconditiondialog.ui \
    src/locationconditiondialog.ui \
    src/loottableeditordock.ui \
    src/loottableentry.ui \
    src/loottablefunction.ui \
    src/mainwindow.ui \
    src/mcrinvsloteditor.ui \
    src/mcrpredcondition.ui \
    src/newdatapackdialog.ui \
    src/numericinput.ui \
    src/predicatedock.ui \
    src/rawjsontextedit.ui \
    src/settingsdialog.ui \
    src/tabbedcodeeditorinterface.ui \
    src/tagselectordialog.ui \
    src/visualrecipeeditordock.ui

TRANSLATIONS += \
    src/MCDatapacker_vi_VN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/app/app.qrc \
    resource/minecraft/minecraft.qrc \

RC_ICONS = resource/app/icon/favicon.ico

VERSION = 0.3.0
QMAKE_TARGET_COMPANY = IoeCmcomc
QMAKE_TARGET = MCDatapacker
QMAKE_TARGET_PRODUCT = MCDatapacker

DISTFILES += \
    src/MCDatapacker_vi_VN.ts
