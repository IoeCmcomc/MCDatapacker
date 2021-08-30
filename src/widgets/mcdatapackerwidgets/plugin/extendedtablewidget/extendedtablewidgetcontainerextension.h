#ifndef EXTENDEDTABLEWIDGETCONTAINEREXTENSION_H
#define EXTENDEDTABLEWIDGETCONTAINEREXTENSION_H

#include <QDesignerContainerExtension>

#include "../extendedtablewidget/extendedtablewidget.h"

class ExtendedTableWidgetContainerExtension : public QObject,
    public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)

public:
    explicit ExtendedTableWidgetContainerExtension(ExtendedTableWidget *widget,
                                                   QObject *parent);

    void addWidget(QWidget *widget) override;
    int count() const override;
    int currentIndex() const override;
    void insertWidget(int index, QWidget *widget) override;
    void remove(int index) override;
    void setCurrentIndex(int index) override;
    QWidget *widget(int index) const override;

private:
    ExtendedTableWidget *myWidget;
};

#endif /* EXTENDEDTABLEWIDGETCONTAINEREXTENSION_H */
