
TEMPLATE = app

TARGET = MCDatapacker

QT += core gui uitools winextras svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 lrelease embed_translations conan_basic_setup
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

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
    codegutter.cpp \
    datapackfileiconprovider.cpp \
    datapacktreeview.cpp \
    datawidgetinterface.cpp \
    dialogdatabutton.cpp \
    disclaimerdialog.cpp \
    entityconditiondialog.cpp \
    filenamedelegate.cpp \
    fileswitcher.cpp \
    globalhelpers.cpp \
    highlighter.cpp \
    imgviewer.cpp \
    inventoryitem.cpp \
    inventoryitemfiltermodel.cpp \
    inventoryslot.cpp \
    inventorysloteditor.cpp \
    itemconditiondialog.cpp \
    jsonhighlighter.cpp \
    linenumberarea.cpp \
    locationconditiondialog.cpp \
    loottablecondition.cpp \
    loottableeditordock.cpp \
    loottableentry.cpp \
    loottablefunction.cpp \
    loottablepool.cpp \
    main.cpp \
    mainwindow.cpp \
    mcfunctionhighlighter.cpp \
    modelfunctions.cpp \
    newdatapackdialog.cpp \
    numberprovider.cpp \
    numberproviderdelegate.cpp \
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
    parsers/command/visitors/nodeformatter.cpp \
    parsers/command/visitors/nodevisitor.cpp \
    predicatedock.cpp \
    problemarea.cpp \
    rawjsontextedit.cpp \
    settingsdialog.cpp \
    stackedwidget.cpp \
    statusbar.cpp \
    stripedscrollbar.cpp \
    tabbeddocumentinterface.cpp \
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
    codegutter.h \
    datapackfileiconprovider.h \
    datapacktreeview.h \
    datawidgetinterface.h \
    dialogdatabutton.h \
    disclaimerdialog.h \
    entityconditiondialog.h \
    filenamedelegate.h \
    fileswitcher.h \
    globalhelpers.h \
    highlighter.h \
    imgviewer.h \
    inventoryitem.h \
    inventoryitemfiltermodel.h \
    inventoryslot.h \
    inventorysloteditor.h \
    itemconditiondialog.h \
    jsonhighlighter.h \
    linenumberarea.h \
    locationconditiondialog.h \
    loottablecondition.h \
    loottableeditordock.h \
    loottableentry.h \
    loottablefunction.h \
    loottablepool.h \
    mainwindow.h \
    mcfunctionhighlighter.h \
    modelfunctions.h \
    newdatapackdialog.h \
    numberprovider.h \
    numberproviderdelegate.h \
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
    parsers/command/visitors/nodeformatter.h \
    parsers/command/visitors/nodevisitor.h \
    predicatedock.h \
    problemarea.h \
    rawjsontextedit.h \
    settingsdialog.h \
    stackedwidget.h \
    statusbar.h \
    stripedscrollbar.h \
    tabbeddocumentinterface.h \
    tagselectordialog.h \
    truefalsebox.h \
    vieweventfilter.h \
    visualrecipeeditordock.h

FORMS += \
    aboutdialog.ui \
    blockitemselectordialog.ui \
    datawidgetinterface.ui \
    dialogdatabutton.ui \
    disclaimerdialog.ui \
    entityconditiondialog.ui \
    itemconditiondialog.ui \
    locationconditiondialog.ui \
    loottablecondition.ui \
    loottableeditordock.ui \
    loottableentry.ui \
    loottablefunction.ui \
    loottablepool.ui \
    mainwindow.ui \
    inventorysloteditor.ui \
    newdatapackdialog.ui \
    numberprovider.ui \
    predicatedock.ui \
    rawjsontextedit.ui \
    settingsdialog.ui \
    tabbeddocumentinterface.ui \
    tagselectordialog.ui \
    visualrecipeeditordock.ui

TRANSLATIONS += \
    ../translations/MCDatapacker_en.ts \
    ../translations/MCDatapacker_vi.ts \
    ../translations/qt_vi.ts \
    ../translations/qtbase_vi.ts \
    ../translations/qtmultimedia_vi.ts \
    ../translations/qtscript_vi.ts \
    ../translations/qtxmlpatterns_vi.ts

RESOURCES += \
    ../resource/app/app.qrc \
    ../resource/minecraft/info/1.15/1.15.qrc \
    ../resource/minecraft/info/1.16/1.16.qrc \
    ../resource/minecraft/info/1.17/1.17.qrc \
    ../resource/minecraft/minecraft.qrc

DISTFILES += \
    ../lib/QFindDialogs/LICENSE

RC_ICONS = ../resource/app/icon/favicon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

VERSION = 0.4.0
QMAKE_TARGET_COMPANY = IoeCmcomc
QMAKE_TARGET = MCDatapacker
QMAKE_TARGET_PRODUCT = MCDatapacker


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/json/release/ -ljson
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/json/debug/ -ljson
else:unix: LIBS += -L$$OUT_PWD/../lib/json/ -ljson

INCLUDEPATH += $$PWD/../lib/json \
    $$PWD/../lib/json/json/single_include
DEPENDPATH += $$PWD/../lib/json

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/json/release/libjson.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/json/debug/libjson.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/json/release/json.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/json/debug/json.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/json/libjson.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/lru-cache/release/ -llru-cache
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/lru-cache/debug/ -llru-cache
else:unix: LIBS += -L$$OUT_PWD/../lib/lru-cache/ -llru-cache

INCLUDEPATH += $$PWD/../lib/lru-cache \
    $$PWD/../lib/lru-cache/lru-cache/include
DEPENDPATH += $$PWD/../lib/lru-cache

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/lru-cache/release/liblru-cache.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/lru-cache/debug/liblru-cache.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/lru-cache/release/lru-cache.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/lru-cache/debug/lru-cache.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/lru-cache/liblru-cache.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/nbt/release/ -lnbt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/nbt/debug/ -lnbt
else:unix: LIBS += -L$$OUT_PWD/../lib/nbt/ -lnbt

INCLUDEPATH += $$PWD/../lib/nbt \
    $$PWD/../lib/nbt/nbt-cpp/include
DEPENDPATH += $$PWD/../lib/nbt

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/nbt/release/libnbt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/nbt/debug/libnbt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/nbt/release/nbt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/nbt/debug/nbt.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/nbt/libnbt.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/QFindDialogs/release/ -lQFindDialogs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/QFindDialogs/debug/ -lQFindDialogs
else:unix: LIBS += -L$$OUT_PWD/../lib/QFindDialogs/ -lQFindDialogs

INCLUDEPATH += $$PWD/../lib/QFindDialogs
DEPENDPATH += $$PWD/../lib/QFindDialogs

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QFindDialogs/release/libQFindDialogs.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QFindDialogs/debug/libQFindDialogs.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QFindDialogs/release/QFindDialogs.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/QFindDialogs/debug/QFindDialogs.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/QFindDialogs/libQFindDialogs.a

#message($$INCLUDEPATH)
