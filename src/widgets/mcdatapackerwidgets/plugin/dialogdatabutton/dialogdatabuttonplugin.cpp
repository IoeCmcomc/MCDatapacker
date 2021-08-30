#include "../dialogdatabutton/dialogdatabutton.h"
#include "dialogdatabutton/dialogdatabuttonplugin.h"

#include <QtPlugin>

DialogDataButtonPlugin::DialogDataButtonPlugin(QObject *parent)
    : QObject(parent) {
    m_initialized = false;
}

void DialogDataButtonPlugin::initialize(
    QDesignerFormEditorInterface * /* core */) {
    if (m_initialized)
        return;

    /* Add extension registrations, etc. here */

    m_initialized = true;
}

bool DialogDataButtonPlugin::isInitialized() const {
    return m_initialized;
}

QWidget *DialogDataButtonPlugin::createWidget(QWidget *parent) {
    return new DialogDataButton(parent);
}

QString DialogDataButtonPlugin::name() const {
    return QLatin1String("DialogDataButton");
}

QString DialogDataButtonPlugin::group() const {
    return QLatin1String("MCDatapacker Widgets");
}

QIcon DialogDataButtonPlugin::icon() const {
    return QIcon();
}

QString DialogDataButtonPlugin::toolTip() const {
    return QString();
}

QString DialogDataButtonPlugin::whatsThis() const {
    return QString();
}

bool DialogDataButtonPlugin::isContainer() const {
    return false;
}

QString DialogDataButtonPlugin::domXml() const {
    return QLatin1String(
        "<widget class=\"DialogDataButton\" name=\"dialogDataButton\">\n</widget>\n");
}

QString DialogDataButtonPlugin::includeFile() const {
    return QLatin1String("dialogdatabutton.h");
}

