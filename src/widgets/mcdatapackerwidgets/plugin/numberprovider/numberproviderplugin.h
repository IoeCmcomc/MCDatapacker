#ifndef PLUGIN_NUMBERPROVIDER_NUMBERPROVIDERPLUGIN_H
#define PLUGIN_NUMBERPROVIDER_NUMBERPROVIDERPLUGIN_H

#include <QDesignerCustomWidgetInterface>

class NumberProviderPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)


public:
    NumberProviderPlugin(QObject *parent = 0);

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
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif // PLUGIN_NUMBERPROVIDER_NUMBERPROVIDERPLUGIN_H
