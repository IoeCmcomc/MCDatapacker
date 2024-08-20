
TEMPLATE = app

TARGET = MCDatapacker

QT += core gui uitools svg widgets-private gui-private testlib
win32:QT += winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 lrelease embed_translations
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

win32:LIBS += -lDwmapi

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Application-specific defines
DEFINES += MCFUNCTIONPARSER_USE_CACHE PARSENODE_REPRPRINTER

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += \
    aboutdialog.cpp \
    advancementitem.cpp \
    advancementtab.cpp \
    advancementtabdock.cpp \
    basecondition.cpp \
    blockitemselectordialog.cpp \
    codeeditor.cpp \
    codefile.cpp \
    codegutter.cpp \
    codepalette.cpp \
    darkfusionstyle.cpp \
    datapackfileiconprovider.cpp \
    datapacktreeview.cpp \
    datawidgetcontroller.cpp \
    datawidgetinterface.cpp \
    disclaimerdialog.cpp \
    entityconditiondialog.cpp \
    entitynamestextobjectdialog.cpp \
    filenamedelegate.cpp \
    fileswitcher.cpp \
    findandreplacedock.cpp \
    game.cpp \
    gameinfomodel.cpp \
    globalhelpers.cpp \
    highlighter.cpp \
    imgviewer.cpp \
    inventoryitem.cpp \
    inventoryitemfiltermodel.cpp \
    inventoryslot.cpp \
    inventorysloteditor.cpp \
    itemconditiondialog.cpp \
    itemmodifierdock.cpp \
    jmchighlighter.cpp \
    jsonhighlighter.cpp \
    layouttreenode.cpp \
    linenumberarea.cpp \
    locationconditiondialog.cpp \
    loottable.cpp \
    loottablecondition.cpp \
    loottableeditordock.cpp \
    loottableentry.cpp \
    loottablefunction.cpp \
    loottablepool.cpp \
    main.cpp \
    mainwindow.cpp \
    mcbuildhighlighter.cpp \
    mcfunctionhighlighter.cpp \
    modelfunctions.cpp \
    nbttextobjectdialog.cpp \
    newdatapackdialog.cpp \
    norwegianwoodstyle.cpp \
    numberproviderdialog.cpp \
    parsers/command/mcfunctionparser.cpp \
    parsers/command/minecraftparser.cpp \
    parsers/command/nodes/gamemodenode.cpp \
    parsers/command/nodes/inlinableresourcenode.cpp \
    parsers/command/nodes/internalregexpatternnode.cpp \
    parsers/command/nodes/itempredicatematchnode.cpp \
    parsers/command/nodes/listnode.cpp \
    parsers/command/nodes/macronode.cpp \
    parsers/command/nodes/stylenode.cpp \
    parsers/command/nodes/uuidnode.cpp \
    parsers/command/re2c_generated_functions.cpp \
    parsers/command/nodes/anglenode.cpp \
    parsers/command/nodes/argumentnode.cpp \
    parsers/command/nodes/axesnode.cpp \
    parsers/command/nodes/blockstatenode.cpp \
    parsers/command/nodes/componentnode.cpp \
    parsers/command/nodes/entitynode.cpp \
    parsers/command/nodes/filenode.cpp \
    parsers/command/nodes/floatrangenode.cpp \
    parsers/command/nodes/intrangenode.cpp \
    parsers/command/nodes/itemstacknode.cpp \
    parsers/command/nodes/literalnode.cpp \
    parsers/command/nodes/mapnode.cpp \
    parsers/command/nodes/nbtnodes.cpp \
    parsers/command/nodes/nbtpathnode.cpp \
    parsers/command/nodes/parsenode.cpp \
    parsers/command/nodes/particlenode.cpp \
    parsers/command/nodes/resourcelocationnode.cpp \
    parsers/command/nodes/rootnode.cpp \
    parsers/command/nodes/singlevaluenode.cpp \
    parsers/command/nodes/stringnode.cpp \
    parsers/command/nodes/swizzlenode.cpp \
    parsers/command/nodes/targetselectornode.cpp \
    parsers/command/nodes/timenode.cpp \
    parsers/command/parsenodecache.cpp \
    parsers/command/schema/schemaargumentnode.cpp \
    parsers/command/schema/schemaliteralnode.cpp \
    parsers/command/schema/schemaloader.cpp \
    parsers/command/schema/schemanode.cpp \
    parsers/command/schema/schemarootnode.cpp \
    parsers/command/schemaparser.cpp \
    parsers/command/visitors/completionprovider.cpp \
    parsers/command/visitors/nodecounter.cpp \
    parsers/command/visitors/nodeformatter.cpp \
    parsers/command/visitors/nodevisitor.cpp \
    parsers/command/visitors/overloadnodevisitor.cpp \
    parsers/command/visitors/reprprinter.cpp \
    parsers/command/visitors/sourceprinter.cpp \
    parsers/jsonparser.cpp \
    parsers/linesplitter.cpp \
    parsers/parser.cpp \
    platforms/windows_specific.cpp \
    predicatedock.cpp \
    problemarea.cpp \
    rawjsontextedit.cpp \
    rawjsontexteditor.cpp \
    rawjsontextobjectinterface.cpp \
    scoreboardtextobjectdialog.cpp \
    settingsdialog.cpp \
    stackedwidget.cpp \
    statisticsdialog.cpp \
    statusbar.cpp \
    stringvectormodel.cpp \
    stripedscrollbar.cpp \
#    stylesheetreapplier.cpp \
    tabbeddocumentinterface.cpp \
    tagselectordialog.cpp \
    translatedtextobjectdialog.cpp \
    truefalsebox.cpp \
    vanilladatapackdock.cpp \
    vanilladatapackmodel.cpp \
    vieweventfilter.cpp \
    visualrecipeeditordock.cpp

HEADERS += \
    aboutdialog.h \
    abstracttextobjectdialog.h \
    advancementitem.h \
    advancementtab.h \
    advancementtabdock.h \
    basecondition.h \
    blockitemselectordialog.h \
    codeeditor.h \
    codefile.h \
    codegutter.h \
    codepalette.h \
    darkfusionstyle.h \
    datapackfileiconprovider.h \
    datapacktreeview.h \
    datawidgetcontroller.h \
    datawidgetinterface.h \
    disclaimerdialog.h \
    entityconditiondialog.h \
    entitynamestextobjectdialog.h \
    filenamedelegate.h \
    fileswitcher.h \
    findandreplacedock.h \
    game.h \
    gameconstants.h \
    gameinfomodel.h \
    globalhelpers.h \
    highlighter.h \
    imgviewer.h \
    inventoryitem.h \
    inventoryitemfiltermodel.h \
    inventoryslot.h \
    inventorysloteditor.h \
    itemconditiondialog.h \
    itemmodifierdock.h \
    jmchighlighter.h \
    jsonhighlighter.h \
    layouttreenode.h \
    linenumberarea.h \
    locationconditiondialog.h \
    loottable.h \
    loottablecondition.h \
    loottableeditordock.h \
    loottableentry.h \
    loottablefunction.h \
    loottablepool.h \
    mainwindow.h \
    mcbuildhighlighter.h \
    mcdatapacker_pch.h \
    mcfunctionhighlighter.h \
    modelfunctions.h \
    nbttextobjectdialog.h \
    newdatapackdialog.h \
    norwegianwoodstyle.h \
    numberproviderdialog.h \
    parsers/command/mcfunctionparser.h \
    parsers/command/nodes/anglenode.h \
    parsers/command/nodes/argumentnode.h \
    parsers/command/nodes/axesnode.h \
    parsers/command/nodes/blockstatenode.h \
    parsers/command/nodes/componentnode.h \
    parsers/command/nodes/entitynode.h \
    parsers/command/nodes/filenode.h \
    parsers/command/nodes/floatrangenode.h \
    parsers/command/nodes/gamemodenode.h \
    parsers/command/nodes/inlinableresourcenode.h \
    parsers/command/nodes/internalregexpatternnode.h \
    parsers/command/nodes/intrangenode.h \
    parsers/command/nodes/itempredicatematchnode.h \
    parsers/command/nodes/itemstacknode.h \
    parsers/command/nodes/listnode.h \
    parsers/command/nodes/literalnode.h \
    parsers/command/nodes/macronode.h \
    parsers/command/nodes/mapnode.h \
    parsers/command/nodes/nbtnodes.h \
    parsers/command/nodes/nbtpathnode.h \
    parsers/command/nodes/parsenode.h \
    parsers/command/nodes/particlenode.h \
    parsers/command/nodes/rangenode.h \
    parsers/command/nodes/resourcelocationnode.h \
    parsers/command/nodes/rootnode.h \
    parsers/command/nodes/singlevaluenode.h \
    parsers/command/nodes/stringnode.h \
    parsers/command/nodes/stylenode.h \
    parsers/command/nodes/swizzlenode.h \
    parsers/command/nodes/targetselectornode.h \
    parsers/command/nodes/timenode.h \
    parsers/command/nodes/uuidnode.h \
    parsers/command/parsenodecache.h \
    parsers/command/minecraftparser.h \
    parsers/command/re2c_functions.re \
    parsers/command/re2c_generated_functions.h \
    parsers/command/schema/schemaargumentnode.h \
    parsers/command/schema/schemaliteralnode.h \
    parsers/command/schema/schemaloader.h \
    parsers/command/schema/schemanode.h \
    parsers/command/schema/schemarootnode.h \
    parsers/command/schemaparser.h \
    parsers/command/visitors/completionprovider.h \
    parsers/command/visitors/nodecounter.h \
    parsers/command/visitors/nodeformatter.h \
    parsers/command/visitors/overloadnodevisitor.h \
    parsers/command/visitors/reprprinter.h \
    parsers/command/visitors/sourceprinter.h \
    parsers/jsonparser.h \
    parsers/linesplitter.h \
    parsers/parser.h \
    platforms/windows_specific.h \
    predicatedock.h \
    problemarea.h \
    rawjsontextedit.h \
    rawjsontexteditor.h \
    rawjsontextobjectinterface.h \
    scoreboardtextobjectdialog.h \
    settingsdialog.h \
    stackedwidget.h \
    statisticsdialog.h \
    statusbar.h \
    stringvectormodel.h \
    stripedscrollbar.h \
#    stylesheetreapplier.h \ # Already added in mcdatapackerwidgets.pri
    tabbeddocumentinterface.h \
    tagselectordialog.h \
    translatedtextobjectdialog.h \
    truefalsebox.h \
    vanilladatapackdock.h \
    vanilladatapackmodel.h \
    vieweventfilter.h \
    visualrecipeeditordock.h

FORMS += \
    aboutdialog.ui \
    advancementtabdock.ui \
    blockitemselectordialog.ui \
    datawidgetinterface.ui \
    disclaimerdialog.ui \
    entityconditiondialog.ui \
    entitynamestextobjectdialog.ui \
    findandreplacedock.ui \
    itemconditiondialog.ui \
    itemmodifierdock.ui \
    locationconditiondialog.ui \
    loottable.ui \
    loottablecondition.ui \
    loottableeditordock.ui \
    loottableentry.ui \
    loottablefunction.ui \
    loottablepool.ui \
    mainwindow.ui \
    inventorysloteditor.ui \
    nbttextobjectdialog.ui \
    newdatapackdialog.ui \
    numberproviderdialog.ui \
    predicatedock.ui \
    rawjsontexteditor.ui \
    scoreboardtextobjectdialog.ui \
    settingsdialog.ui \
    statisticsdialog.ui \
    tabbeddocumentinterface.ui \
    tagselectordialog.ui \
    translatedtextobjectdialog.ui \
    vanilladatapackdock.ui \
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
    ../res/app/app.qrc \
    ../res/mc/info/1.15/1.15.qrc \
    ../res/mc/info/1.16/1.16.qrc \
    ../res/mc/info/1.17/1.17.qrc \
    ../res/mc/info/1.18/1.18.qrc \
    ../res/mc/info/1.18.2/1.18.2.qrc \
    ../res/mc/info/1.19/1.19.qrc \
    ../res/mc/info/1.19.3/1.19.3.qrc \
    ../res/mc/info/1.19.4/1.19.4.qrc \
    ../res/mc/info/1.20/1.20.qrc \
    ../res/mc/info/1.20.2/1.20.2.qrc \
    ../res/mc/info/1.20.4/1.20.4.qrc \
    ../res/mc/info/1.20.6/1.20.6.qrc \
    ../res/mc/info/1.21/1.21.qrc \
    ../res/mc/info/1.15/1.15-data-json.qrc \
    ../res/mc/info/1.16/1.16-data-json.qrc \
    ../res/mc/info/1.17/1.17-data-json.qrc \
    ../res/mc/info/1.18/1.18-data-json.qrc \
    ../res/mc/info/1.18.2/1.18.2-data-json.qrc \
    ../res/mc/info/1.19/1.19-data-json.qrc \
    ../res/mc/info/1.19.3/1.19.3-data-json.qrc \
    ../res/mc/info/1.19.4/1.19.4-data-json.qrc \
    ../res/mc/info/1.20/1.20-data-json.qrc \
    ../res/mc/info/1.20.2/1.20.2-data-json.qrc \
    ../res/mc/info/1.20.4/1.20.4-data-json.qrc \
    ../res/mc/info/1.20.6/1.20.6-data-json.qrc \
    ../res/mc/info/1.21/1.21-data-json.qrc \
    ../res/mc/minecraft.qrc \
    ../res/app/icons/default/default.qrc

DISTFILES += \
    ../lib/QFindDialogs/LICENSE \
    ../res/app/fonts/LICENSE_Monocraft.txt

RC_ICONS = ../res/app/icon/favicon.ico

include(widgets/mcdatapackerwidgets/mcdatapackerwidgets.pri)

include($$PWD/../lib/QSimpleUpdater/QSimpleUpdater.pri)
include($$PWD/../lib/libqdark/libqdark.pri)
include($$PWD/../lib/json/json.pri)
include($$PWD/../lib/uberswitch/uberswitch.pri)
include($$PWD/../lib/lru-cache/lru-cache.pri)
include($$PWD/../lib/miniz/miniz.pri)

DEFINES -= QSU_INCLUDE_MOC

#PRECOMPILED_HEADER = mcdatapacker_pch.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

VERSION_MAJOR = 0
VERSION_MINOR = 10
VERSION_PATCH = 0

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}
QMAKE_TARGET_COMPANY = IoeCmcomc
QMAKE_TARGET = MCDatapacker
QMAKE_TARGET_PRODUCT = MCDatapacker
QMAKE_TARGET_DESCRIPTION = MCDatapacker - Minecraft datapack editor
QMAKE_TARGET_COPYRIGHT = \\251 2020 - 2024 IoeCmcomc

DEFINES += APP_VERSION=\\\"$$VERSION\\\" \
    APP_VERSION_MAJOR=$$VERSION_MAJOR \
    APP_VERSION_MINOR=$$VERSION_MINOR \
    APP_VERSION_PATCH=$$VERSION_PATCH

QMAKE_RESOURCE_FLAGS += -threshold 40 -compress 9 -compress-algo zlib

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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/qlementine/release/ -lqlementine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/qlementine/debug/ -lqlementine
else:unix: LIBS += -L$$OUT_PWD/../lib/qlementine/ -lqlementine

INCLUDEPATH += $$PWD/../lib/qlementine \
    $$PWD/../lib/qlementine/qlementine/lib/include
DEPENDPATH += $$PWD/../lib/qlementine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/qlementine/release/libqlementine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/qlementine/debug/libqlementine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/qlementine/release/qlementine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/qlementine/debug/qlementine.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/qlementine/libqlementine.a


win32-msvc*: {
    LIBS += -lAdvAPI32
    # CONFIG(force_debug_info) {
    #     DEFINES += _DISABLE_VECTOR_ANNOTATION=1 _DISABLE_STRING_ANNOTATION=1
    #     QMAKE_CXXFLAGS += -fsanitize=address
    # }
    QMAKE_LFLAGS_DEBUG += /DEBUG:FASTLINK
}

#message($$INCLUDEPATH)
