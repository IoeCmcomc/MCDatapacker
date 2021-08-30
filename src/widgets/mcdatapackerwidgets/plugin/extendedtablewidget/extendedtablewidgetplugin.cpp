#include "../extendedtablewidget/extendedtablewidget.h"

#include "extendedtablewidget/extendedtablewidgetplugin.h"
#include "extendedtablewidgetextensionfactory.h"

#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QDesignerContainerExtension>
#include <QExtensionManager>

ExtendedTableWidgetPlugin::ExtendedTableWidgetPlugin(QObject *parent)
    : QObject(parent) {
}

void ExtendedTableWidgetPlugin::initialize(
    QDesignerFormEditorInterface *formEditor) {
    if (m_initialized)
        return;

    QExtensionManager *manager = formEditor->extensionManager();

    QExtensionFactory *factory =
        new ExtendedTableWidgetExtensionFactory(manager);

    Q_ASSERT(manager != 0);
    manager->registerExtensions(factory, Q_TYPEID(QDesignerContainerExtension));

    m_initialized = true;
}

bool ExtendedTableWidgetPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *ExtendedTableWidgetPlugin::createWidget(QWidget *parent) {
    auto *w = new ExtendedTableWidget(parent);

    return w;
}

QString ExtendedTableWidgetPlugin::name() const {
    return QLatin1String("ExtendedTableWidget");
}

QString ExtendedTableWidgetPlugin::group() const {
    return QLatin1String("MCDatapacker Widgets");
}

QIcon ExtendedTableWidgetPlugin::icon() const {
    return QIcon();
}

QString ExtendedTableWidgetPlugin::toolTip() const {
    return QString();
}

QString ExtendedTableWidgetPlugin::whatsThis() const {
    return QString();
}

bool ExtendedTableWidgetPlugin::isContainer() const {
    return true;
}

QString ExtendedTableWidgetPlugin::domXml() const {
    return QLatin1String(
        R"(
<ui language="c++">
    <widget class="ExtendedTableWidget" name="extendedTableWidget">
        <widget class="QTableWidget" name="table" />
        <widget class="QFrame" name="container" />
    </widget>
    <customwidgets>
        <customwidget>
            <class>ExtendedTableWidget</class>
            <extends>QWidget</extends>
            <addpagemethod>setContainer</addpagemethod>
        </customwidget>
    </customwidgets>
</ui>
)");
}

QString ExtendedTableWidgetPlugin::includeFile() const {
    return QLatin1String("extendedtablewidget.h");
}

