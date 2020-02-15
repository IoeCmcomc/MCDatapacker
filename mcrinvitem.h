#ifndef MCRINVITEM_H
#define MCRINVITEM_H

#include <QLabel>
#include <QString>

class MCRInvItem : public QLabel
{
    //Q_OBJECT

public:
    MCRInvItem(QWidget *parent = nullptr, QString id ="");
    static QMap<QString, QString> itemList();

    QString namespacedID = "";
    QString getName();
    void setName(const QString &name);

private:
    QString name;

    void setupItem();
};

#endif // MCRINVITEM_H
