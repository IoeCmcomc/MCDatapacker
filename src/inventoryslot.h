#ifndef INVENTORYSLOT_H
#define INVENTORYSLOT_H

#include "inventoryitem.h"

#include <QFrame>
#include <QLabel>
#include <QTimer>

class InventorySlot : public QFrame
{
    Q_OBJECT

public:
    enum AcceptPolicy : uchar {
        AcceptMultiple = 1,
        AcceptTags     = 2,
    };
    Q_DECLARE_FLAGS(AcceptPolicies, AcceptPolicy);

    enum class SelectCategory : uchar {
        ObtainableItems, /* All regular items and blocks wihch are representable
                            as items are selectable */
        Blocks,          /* Only blocks are selectable */
    };

    static QTimer m_timer;

    explicit InventorySlot(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    void setItems(const QVector<InventoryItem> &items);
    void setItem(const InventoryItem &item);
    void setItem(InventoryItem &&item);
    void appendItem(const InventoryItem &item);
    void appendItem(InventoryItem &&item);
    void appendItems(const QVector<InventoryItem> &items);
    void insertItem(const int index, const InventoryItem &item);
    void removeItem(const int index);
    int removeItem(const InventoryItem &item);
    void clearItems();
    InventoryItem &getItem(const int index = 0);
    QVector<InventoryItem> &getItems();
    inline int itemCount() const {
        return items.size();
    }
    inline int isEmpty() const {
        return items.isEmpty();
    }

    bool checkAcceptableItem(const InventoryItem &item);

    bool getIsCreative() const;
    void setIsCreative(bool value);

    QString itemNamespacedID(const int index = 0);
    QString itemName(const int index         = 0);

    void setBackground(const QString &color = QStringLiteral("#8B8B8B"));

    bool getAcceptTag() const;
    void setAcceptTag(bool value);

    bool getAcceptMultiple() const;
    void setAcceptMultiple(bool value);

    SelectCategory selectCategory() const;
    void setSelectCategory(const SelectCategory &selectCategory);

signals:
    void itemChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCustomContextMenu(const QPoint &point);
    void onItemChanged();
    void onTimerTimeout();

private:
    QPoint mousePressPos;
    QVector<InventoryItem> items;
    int curItemIndex                = -1;
    AcceptPolicies m_acceptPolicies = { AcceptMultiple, AcceptTags };
    SelectCategory m_selectCategory = SelectCategory::ObtainableItems;
    bool isCreative                 = false;
    bool isDragged                  = false;
    bool itemHidden                 = false;

    void startDrag(QMouseEvent *event);
    void hideItem();
    void showItem();
    QString toolTipText();
    bool checkAcceptableItems(const QVector<InventoryItem> &items);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InventorySlot::AcceptPolicies)

#endif /* INVENTORYSLOT_H */
