#include "extendedtablewidgetextensionfactory.h"
#include "extendedtablewidgetcontainerextension.h"

#include "../extendedtablewidget/extendedtablewidget.h"

ExtendedTableWidgetExtensionFactory::ExtendedTableWidgetExtensionFactory(
    QExtensionManager *parent)
    : QExtensionFactory(parent) {
}

QObject *ExtendedTableWidgetExtensionFactory::createExtension(
    QObject *object, const QString &iid, QObject *parent) const {
    ExtendedTableWidget *widget = qobject_cast<ExtendedTableWidget*>(object);

    if (widget && (iid == Q_TYPEID(QDesignerContainerExtension)))
        return new ExtendedTableWidgetContainerExtension(widget, parent);

    return nullptr;
}
