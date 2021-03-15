
TEMPLATE = app

TARGET = MCDatapacker

QT += core gui uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 lrelease embed_translations conan_basic_setup

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
    disclaimerdialog.cpp \
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
    parsers/command/minecraftparser.cpp \
    parsers/command/nodes/argumentnode.cpp \
    parsers/command/nodes/axesnode.cpp \
    parsers/command/nodes/axisnode.cpp \
    parsers/command/nodes/blockstatenode.cpp \
    parsers/command/nodes/boolnode.cpp \
    parsers/command/nodes/componentnode.cpp \
    parsers/command/nodes/doublenode.cpp \
    parsers/command/nodes/entitynode.cpp \
    parsers/command/nodes/floatnode.cpp \
    parsers/command/nodes/floatrangenode.cpp \
    parsers/command/nodes/integernode.cpp \
    parsers/command/nodes/intrangenode.cpp \
    parsers/command/nodes/itemstacknode.cpp \
    parsers/command/nodes/literalnode.cpp \
    parsers/command/nodes/mapnode.cpp \
    parsers/command/nodes/multimapnode.cpp \
    parsers/command/nodes/nbtnodes.cpp \
    parsers/command/nodes/nbtpathnode.cpp \
    parsers/command/nodes/parsenode.cpp \
    parsers/command/nodes/particlenode.cpp \
    parsers/command/nodes/resourcelocationnode.cpp \
    parsers/command/nodes/rootnode.cpp \
    parsers/command/nodes/similaraxesnodes.cpp \
    parsers/command/nodes/similarresourcelocationnodes.cpp \
    parsers/command/nodes/similarstringnodes.cpp \
    parsers/command/nodes/stringnode.cpp \
    parsers/command/nodes/swizzlenode.cpp \
    parsers/command/nodes/targetselectornode.cpp \
    parsers/command/nodes/timenode.cpp \
    parsers/command/nodes/uuidnode.cpp \
    parsers/command/parsenodecache.cpp \
    parsers/command/parser.cpp \
    predicatedock.cpp \
    rawjsontextedit.cpp \
    settingsdialog.cpp \
    stackedwidget.cpp \
    tabbedcodeeditorinterface.cpp \
    tagselectordialog.cpp \
    truefalsebox.cpp \
    vieweventfilter.cpp \
    visualrecipeeditordock.cpp \

HEADERS += \
    aboutdialog.h \
    basecondition.h \
    blockitemselectordialog.h \
    codeeditor.h \
    codefile.h \
    datapackfileiconprovider.h \
    datapacktreeview.h \
    dialogdatabutton.h \
    disclaimerdialog.h \
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
    numericinput.cpp.GANRTu \
    numericinput.h \
    optionalspinbox.h \
    parsers/command/nodes/argumentnode.h \
    parsers/command/nodes/axesnode.h \
    parsers/command/nodes/axisnode.h \
    parsers/command/nodes/blockstatenode.h \
    parsers/command/nodes/boolnode.h \
    parsers/command/nodes/componentnode.h \
    parsers/command/nodes/doublenode.h \
    parsers/command/nodes/entitynode.h \
    parsers/command/nodes/floatnode.h \
    parsers/command/nodes/floatrangenode.h \
    parsers/command/nodes/integernode.h \
    parsers/command/nodes/intrangenode.h \
    parsers/command/nodes/itemstacknode.h \
    parsers/command/nodes/literalnode.h \
    parsers/command/nodes/mapnode.h \
    parsers/command/nodes/multimapnode.h \
    parsers/command/nodes/nbtnodes.h \
    parsers/command/nodes/nbtpathnode.h \
    parsers/command/nodes/parsenode.h \
    parsers/command/nodes/particlenode.h \
    parsers/command/nodes/rangenode.h \
    parsers/command/nodes/resourcelocationnode.h \
    parsers/command/nodes/rootnode.h \
    parsers/command/nodes/similaraxesnodes.h \
    parsers/command/nodes/similarresourcelocationnodes.h \
    parsers/command/nodes/similarstringnodes.h \
    parsers/command/nodes/stringnode.h \
    parsers/command/nodes/swizzlenode.h \
    parsers/command/nodes/targetselectornode.h \
    parsers/command/nodes/timenode.h \
    parsers/command/nodes/uuidnode.h \
    parsers/command/parsenodecache.h \
    parsers/command/parser.h \
    parsers/command/minecraftparser.h \
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
    disclaimerdialog.ui \
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
    ../resource/app/app.qrc \
    ../resource/minecraft/minecraft.qrc

RC_ICONS = ../resource/app/icon/favicon.ico

VERSION = 0.3.0
QMAKE_TARGET_COMPANY = IoeCmcomc
QMAKE_TARGET = MCDatapacker
QMAKE_TARGET_PRODUCT = MCDatapacker

DISTFILES += \
    ../lib/QFindDialogs/LICENSE \
    MCDatapacker_vi_VN.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -llib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lliblib
else:unix: LIBS += -L$$OUT_PWD/../lib/ -llib

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib

INCLUDEPATH += $$PWD/../lib/lru-cache/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/release/liblib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/liblib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/release/lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/liblib.a
