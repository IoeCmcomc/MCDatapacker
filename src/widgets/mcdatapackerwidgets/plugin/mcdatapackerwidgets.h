#ifndef PLUGIN_MCDATAPACKERWIDGETS_H
#define PLUGIN_MCDATAPACKERWIDGETS_H

#define __clang_major__    11

#include <QtDesigner>
#include <qplugin.h>

class MCDatapackerWidgets : public QObject,
    public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "IoeCmcomc.MCDatapacker.widgets")
#endif /* QT_VERSION >= 0x050000 */

public:
    explicit MCDatapackerWidgets(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif /* PLUGIN_MCDATAPACKERWIDGETS_H */
