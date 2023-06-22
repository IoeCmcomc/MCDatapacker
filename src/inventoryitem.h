#ifndef INVENTORYITEM_H
#define INVENTORYITEM_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QDebug>

class InventoryItem
{
public:
    enum Flag : unsigned int {
        Null      = 0,
        Block     = 1,
        Item      = 2,
        BlockItem = Block | Item,
        Tag       = 4,
    };
    Q_DECLARE_FLAGS(Flags, Flag);

    InventoryItem()                           = default;
    ~InventoryItem()                          = default;
    InventoryItem(const InventoryItem &other) = default;
    InventoryItem(InventoryItem &&other);

    InventoryItem(const QString &id);

    InventoryItem &operator=(const InventoryItem &other) = default;
    InventoryItem &operator=(InventoryItem &&other);;

    bool operator==(const InventoryItem &other);
    bool operator==(const InventoryItem &other) const;

    bool operator!=(const InventoryItem &other);
    bool operator!=(const InventoryItem &other) const;

    bool operator<(const InventoryItem &other) const;

    QString getName() const;
    void setName(const QString &name);

    QString getNamespacedID() const;
    void setNamespacedID(const QString &id);

    bool isBlock() const;
    void setIsBlock(const bool &value);

    bool isItem() const;
    void setIsItem(const bool &value);

    bool isTag() const;

    QPixmap getPixmap() const;
    void setPixmap(const QPixmap &newPixmap);

    QString toolTip() const;

    bool isNull() const;

    Flags getFlags() const;
    void setFlags(const Flags &flags);

    friend QDataStream &operator<<(QDataStream &out, const InventoryItem &obj);
    friend QDataStream &operator>>(QDataStream &in, InventoryItem &obj);


private:
    mutable QPixmap m_pixmap;
    QString m_name;
    QString m_namespacedId;
    Flags m_flags = Flag::Null;

    void setupItem(QString id);
    QPixmap loadPixmap(QString id) const;
};

Q_DECLARE_METATYPE(InventoryItem);
//Q_DECLARE_OPAQUE_POINTER(InventoryItem *)

/*
   QDataStream & operator<<(QDataStream & out, const InventoryItem &obj);
   QDataStream &operator>>(QDataStream &in, InventoryItem &obj);
 */

QDebug operator<<(QDebug debug, const InventoryItem &item);

#endif /* INVENTORYITEM_H */
