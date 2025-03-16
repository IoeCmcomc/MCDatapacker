#ifndef INVENTORYSLOT_H
#define INVENTORYSLOT_H

#include "inventoryitem.h"

#include <QFrame>
#include <QTimer>

class InventorySlot : public QFrame
{
    Q_OBJECT

public:
    enum AcceptPolicy : uchar {
        AcceptItem       = 1,
        AcceptItems      = 2,
        AcceptTag        = 4,
        AcceptTags       = 8,
        AcceptComponents = 16,
    };
    Q_DECLARE_FLAGS(AcceptPolicies, AcceptPolicy);

    enum class SelectCategory : uchar {
        ObtainableItems, /* All regular items and blocks wihch are representable
                            as items are selectable */
        Blocks,          /* Only blocks are selectable */
    };

    static QTimer m_timer;

    explicit InventorySlot(QWidget *parent = nullptr);

    QSize sizeHint() const final;

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

    QString itemNamespacedID(const int index   = 0);
    QString itemName(const int index           = 0);
    QVariantMap itemComponents(const int index = 0);

    void setBackground(const QString &color = QStringLiteral("#8B8B8B"));

    bool getAcceptTag() const;
    void setAcceptTag(bool value);

    bool getAcceptMultiple() const;
    void setAcceptMultiple(bool value);

    bool getAcceptItemsOrTag() const;
    void setAcceptItemsOrTag();

    bool getAcceptComponents() const;
    void setAcceptComponents(bool value);

    SelectCategory selectCategory() const;
    void setSelectCategory(const SelectCategory &selectCategory);

    AcceptPolicies acceptPolicies() const;
    void setAcceptPolicies(const AcceptPolicies &newAcceptPolicies);

signals:
    void itemChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event) final;
    void dragMoveEvent(QDragMoveEvent *event) final;
    void dragLeaveEvent(QDragLeaveEvent *event) final;
    void dropEvent(QDropEvent *event) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void keyPressEvent(QKeyEvent *event) final;
    void paintEvent(QPaintEvent *event) final;

private slots:
    void onCustomContextMenu(const QPoint &point);
    void onItemChanged();
    void onTimerTimeout();

private:
    QPoint mousePressPos;
    QVector<InventoryItem> items;
    int curItemIndex                = -1;
    AcceptPolicies m_acceptPolicies =
    { AcceptItem, AcceptItems, AcceptTag, AcceptTags };
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
