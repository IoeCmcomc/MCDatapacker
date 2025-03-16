#ifndef PLUGIN_IDTAGSELECTORPLUGIN_H
#define PLUGIN_IDTAGSELECTORPLUGIN_H

#include <QDesignerCustomWidgetInterface>

class IdTagSelectorPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)


public:
    IdTagSelectorPlugin(QObject *parent = 0);

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

#endif // PLUGIN_IDTAGSELECTORPLUGIN_H
