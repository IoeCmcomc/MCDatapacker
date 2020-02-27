#ifndef MCRINVITEM_H
#define MCRINVITEM_H

#include <QLabel>
#include <QString>

class MCRInvItem : public QLabel
{
    Q_OBJECT

public:
    MCRInvItem(QWidget *parent = nullptr, QString id = "");
    ~MCRInvItem();
    static QMap<QString, QString> itemList();

    QString getName();
    void setName(const QString &name);

    QString getNamespacedID() const;
    void setNamespacedID(const QString &value);

    bool getHasBlockForm() const;
    void setHasBlockForm(bool value);

private:
    QString name;
    QString namespacedID = "";
    bool hasBlockForm;

    void setupItem();
};

#endif // MCRINVITEM_H
