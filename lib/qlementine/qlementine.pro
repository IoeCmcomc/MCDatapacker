QT = core widgets svg
win32:QT += winextras

TEMPLATE = lib

CONFIG += staticlib c++17

DEFINES += QT_DEFAULT_MAJOR_VERSION=5

HEADERS = \
$$PWD/qlementine/lib/include/oclero/qlementine/Common.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/animation/WidgetAnimation.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/animation/WidgetAnimator.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/animation/WidgetAnimationManager.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/csd/FramelessWindowBehavior.hpp \
# $$PWD/qlementine/lib/include/oclero/qlementine/csd/WindowsTitleBar.hpp \
# $$PWD/qlementine/lib/include/oclero/qlementine/csd/WindowsTitleBarButton.hpp \
# $$PWD/qlementine/lib/include/oclero/qlementine/csd/WindowsTitleBarCommon.hpp \
# $$PWD/qlementine/lib/include/oclero/qlementine/csd/WindowsTitleMenuBarProxyStyle.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/resources/ResourceInitialization.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/style/Delegates.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/style/QlementineStyle.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/style/QlementineStyleOption.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/style/Theme.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/style/ThemeManager.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/tools/ThemeEditor.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/BlurUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/BadgeUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/ColorUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/FontUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/ImageUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/PrimitiveUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/RadiusesF.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/StateUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/StyleUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/utils/WidgetUtils.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/AbstractItemListWidget.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Action.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/ActionButton.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/ColorButton.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/ColorEditor.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/CommandLinkButton.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Expander.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/IconWidget.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Label.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/LineEdit.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Menu.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/NavigationBar.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Popover.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/PopoverButton.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/RoundedFocusFrame.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/SegmentedControl.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/StatusBadgeWidget.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/Switch.hpp \
$$PWD/qlementine/lib/include/oclero/qlementine/widgets/PlainTextEdit.hpp

SOURCES = \
$$PWD/qlementine/lib/src/animation/WidgetAnimator.cpp \
$$PWD/qlementine/lib/src/animation/WidgetAnimationManager.cpp \
$$PWD/qlementine/lib/src/csd/FramelessWindowBehavior.cpp \
# $$PWD/qlementine/lib/src/csd/WindowsTitleBar.cpp \
# $$PWD/qlementine/lib/src/csd/WindowsTitleBarButton.cpp \
# $$PWD/qlementine/lib/src/csd/WindowsTitleMenuBarProxyStyle.cpp \
$$PWD/qlementine/lib/src/resources/ResourceInitialization.cpp \
$$PWD/qlementine/lib/src/style/Delegates.cpp \
$$PWD/qlementine/lib/src/style/EventFilters.cpp \
$$PWD/qlementine/lib/src/style/EventFilters.hpp \
$$PWD/qlementine/lib/src/style/QlementineStyle.cpp \
$$PWD/qlementine/lib/src/style/Theme.cpp \
$$PWD/qlementine/lib/src/style/ThemeManager.cpp \
$$PWD/qlementine/lib/src/tools/ThemeEditor.cpp \
$$PWD/qlementine/lib/src/utils/BadgeUtils.cpp \
$$PWD/qlementine/lib/src/utils/ColorUtils.cpp \
$$PWD/qlementine/lib/src/utils/FontUtils.cpp \
$$PWD/qlementine/lib/src/utils/ImageUtils.cpp \
$$PWD/qlementine/lib/src/utils/PrimitiveUtils.cpp \
$$PWD/qlementine/lib/src/utils/RadiusesF.cpp \
$$PWD/qlementine/lib/src/utils/StateUtils.cpp \
$$PWD/qlementine/lib/src/utils/StyleUtils.cpp \
$$PWD/qlementine/lib/src/utils/WidgetUtils.cpp \
$$PWD/qlementine/lib/src/widgets/AbstractItemListWidget.cpp \
$$PWD/qlementine/lib/src/widgets/Action.cpp \
$$PWD/qlementine/lib/src/widgets/ActionButton.cpp \
$$PWD/qlementine/lib/src/widgets/ColorButton.cpp \
$$PWD/qlementine/lib/src/widgets/ColorEditor.cpp \
$$PWD/qlementine/lib/src/widgets/CommandLinkButton.cpp \
$$PWD/qlementine/lib/src/widgets/Expander.cpp \
$$PWD/qlementine/lib/src/widgets/IconWidget.cpp \
$$PWD/qlementine/lib/src/widgets/Label.cpp \
$$PWD/qlementine/lib/src/widgets/LineEdit.cpp \
$$PWD/qlementine/lib/src/widgets/Menu.cpp \
$$PWD/qlementine/lib/src/widgets/NavigationBar.cpp \
$$PWD/qlementine/lib/src/widgets/Popover.cpp \
$$PWD/qlementine/lib/src/widgets/PopoverButton.cpp \
$$PWD/qlementine/lib/src/widgets/RoundedFocusFrame.cpp \
$$PWD/qlementine/lib/src/widgets/SegmentedControl.cpp \
$$PWD/qlementine/lib/src/widgets/StatusBadgeWidget.cpp \
$$PWD/qlementine/lib/src/widgets/Switch.cpp \
$$PWD/qlementine/lib/src/widgets/PlainTextEdit.cpp

# DISTFILES = $$PWD/QFindDialogs/LICENSE

INCLUDEPATH += $$PWD/qlementine/lib/include
# DEPENDPATH += $$PWD/qlementine/lib/include

RESOURCES += \
    qlementine/lib/resources/qlementine.qrc \
    qlementine/lib/resources/qlementine_font_roboto.qrc
win32:RESOURCES += qlementine/lib/resources/qlementine_font_inter_windows.qrc
else:RESOURCES += qlementine/lib/resources/qlementine_font_inter.qrc

QMAKE_RESOURCE_FLAGS += -threshold 40 -compress 9 -compress-algo zlib
