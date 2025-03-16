#include "../idtagselector/idtagselector.h"
#include "idtagselector/idtagselectorplugin.h"

#include <QtPlugin>

IdTagSelectorPlugin::IdTagSelectorPlugin(QObject *parent)
    : QObject(parent) {
    m_initialized = false;
}

void IdTagSelectorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    /* Add extension registrations, etc. here */

    m_initialized = true;
}

bool IdTagSelectorPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *IdTagSelectorPlugin::createWidget(QWidget *parent) {
    return new IdTagSelector(parent);
}

QString IdTagSelectorPlugin::name() const {
    return QLatin1String("IdTagSelector");
}

QString IdTagSelectorPlugin::group() const {
    return QLatin1String("MCDatapacker Widgets");
}

QIcon IdTagSelectorPlugin::icon() const {
    return QIcon();
}

QString IdTagSelectorPlugin::toolTip() const {
    return QString();
}

QString IdTagSelectorPlugin::whatsThis() const {
    return QString();
}

bool IdTagSelectorPlugin::isContainer() const {
    return false;
}

QString IdTagSelectorPlugin::domXml() const {
    return QLatin1String(
        "<widget class=\"IdTagSelector\" name=\"IdTagSelector\">\n</widget>\n");
}

QString IdTagSelectorPlugin::includeFile() const {
    return QLatin1String("idtagselector.h");
}

