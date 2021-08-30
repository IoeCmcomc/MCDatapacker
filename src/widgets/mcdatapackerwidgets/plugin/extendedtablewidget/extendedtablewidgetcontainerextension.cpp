#include "extendedtablewidgetcontainerextension.h"

#include <QFrame>

ExtendedTableWidgetContainerExtension::ExtendedTableWidgetContainerExtension(
    ExtendedTableWidget *widget, QObject *parent)
    : QObject(parent), myWidget(widget) {
}

void ExtendedTableWidgetContainerExtension::addWidget(QWidget *widget) {
    if (auto *frame = qobject_cast<QFrame*>(widget)) {
        myWidget->setContainer(frame);
    }
}

int ExtendedTableWidgetContainerExtension::count() const {
    return 1;
}

int ExtendedTableWidgetContainerExtension::currentIndex() const {
    return 0;
}

void ExtendedTableWidgetContainerExtension::insertWidget(int, QWidget *) {
}

void ExtendedTableWidgetContainerExtension::remove(int) {
}

void ExtendedTableWidgetContainerExtension::setCurrentIndex(int) {
}

QWidget* ExtendedTableWidgetContainerExtension::widget(int) const {
    return qobject_cast<QWidget*>(myWidget->container());
}
