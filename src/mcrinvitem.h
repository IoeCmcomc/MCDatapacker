#ifndef MCRINVITEM_H
#define MCRINVITEM_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QDebug>

class MCRInvItem
{
public:
    MCRInvItem(void);
    MCRInvItem(const QString &id);
    MCRInvItem(const MCRInvItem &other);
    ~MCRInvItem();

    void init();

    MCRInvItem &operator=(const MCRInvItem &other);
    bool operator==(const MCRInvItem &other);
    bool operator==(const MCRInvItem &other) const;

    bool operator!=(const MCRInvItem &other);
    bool operator!=(const MCRInvItem &other) const;

    bool operator<(const MCRInvItem &other) const;

    QString getName() const;
    void setName(const QString &name);

    QString getNamespacedID() const;
    void setNamespacedID(const QString &id);

    bool getHasBlockForm() const;
    void setHasBlockForm(const bool &value);

    bool getIsTag() const;

    QPixmap getPixmap() const;
    void setPixmap(const QPixmap &value);

    QString toolTip() const;

    bool isEmpty() const;

    /*
       friend QDataStream &operator<<(QDataStream &out, const MCRInvItem &obj);
       friend QDataStream &operator>>(QDataStream &in, MCRInvItem &obj);
     */

private:
    QString name;
    QString namespacedID;
    bool isTag        = false;
    bool m_isEmpty    = true;
    bool hasBlockForm = false;
    QPixmap pixmap;

    void setupItem(QString id);
    void setEmpty(const bool &value);
};

Q_DECLARE_METATYPE(MCRInvItem);
Q_DECLARE_OPAQUE_POINTER(MCRInvItem*)
/*Q_DECLARE_METATYPE(MCRInvItem*); */

QDataStream & operator<<(QDataStream & out, const MCRInvItem &obj);
QDataStream &operator>>(QDataStream &in, MCRInvItem &obj);

QDebug operator<<(QDebug debug, const MCRInvItem &item);

#endif /* MCRINVITEM_H */
