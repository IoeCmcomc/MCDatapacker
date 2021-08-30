#ifndef PLUGIN_EXTENDEDTABLEWIDGET_EXTENDEDTABLEWIDGETPLUGIN_H
#define PLUGIN_EXTENDEDTABLEWIDGET_EXTENDEDTABLEWIDGETPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class ExtendedTableWidgetPlugin : public QObject,
    public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)


public:
    ExtendedTableWidgetPlugin(QObject *parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *formEditor);

private:
    bool m_initialized = false;
};

#endif /* PLUGIN_EXTENDEDTABLEWIDGET_EXTENDEDTABLEWIDGETPLUGIN_H */
