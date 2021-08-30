#include "../numberprovider/numberprovider.h"
#include "numberprovider/numberproviderplugin.h"

#include <QtPlugin>

NumberProviderPlugin::NumberProviderPlugin(QObject *parent)
    : QObject(parent) {
    m_initialized = false;
}

void NumberProviderPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    /* Add extension registrations, etc. here */

    m_initialized = true;
}

bool NumberProviderPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *NumberProviderPlugin::createWidget(QWidget *parent) {
    return new NumberProvider(parent);
}

QString NumberProviderPlugin::name() const {
    return QLatin1String("NumberProvider");
}

QString NumberProviderPlugin::group() const {
    return QLatin1String("MCDatapacker Widgets");
}

QIcon NumberProviderPlugin::icon() const {
    return QIcon();
}

QString NumberProviderPlugin::toolTip() const {
    return QString();
}

QString NumberProviderPlugin::whatsThis() const {
    return QString();
}

bool NumberProviderPlugin::isContainer() const {
    return false;
}

QString NumberProviderPlugin::domXml() const {
    return QLatin1String(
        "<widget class=\"NumberProvider\" name=\"numberProvider\">\n</widget>\n");
}

QString NumberProviderPlugin::includeFile() const {
    return QLatin1String("numberprovider.h");
}

