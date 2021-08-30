#ifndef EXTENDEDTABLEWIDGETEXTENSIONFACTORY_H
#define EXTENDEDTABLEWIDGETEXTENSIONFACTORY_H

#include <QExtensionFactory>
#include <QExtensionManager>

class ExtendedTableWidgetExtensionFactory : public QExtensionFactory
{
    Q_OBJECT
public:
    explicit ExtendedTableWidgetExtensionFactory(
        QExtensionManager *parent = nullptr);

protected:
    QObject *createExtension(QObject *object,
                             const QString &iid,
                             QObject *parent) const override;
};

#endif /* EXTENDEDTABLEWIDGETEXTENSIONFACTORY_H */
