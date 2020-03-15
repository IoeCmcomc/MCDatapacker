#ifndef MCRInvSlot_H
#define MCRInvSlot_H

#include "mcrinvitem.h"

#include <QFrame>
#include <QLabel>

class MCRInvSlot : public QFrame
{
    Q_OBJECT

public:
    explicit MCRInvSlot(QWidget *parent = nullptr,
                        MCRInvItem item = MCRInvItem());

    void setItem(const QVector<MCRInvItem> &items, bool emitSignal    = false);
    void setItem(MCRInvItem item, bool emitSignal                     = false);
    void appendItem(MCRInvItem item, bool emitSignal                  = false);
    void insertItem(const int index, MCRInvItem item, bool emitSignal = false);
    void removeItem(const int index);
    int removeItem(const MCRInvItem item);
    void clearItems(bool emitSignal     = false);
    MCRInvItem &getItem(const int index = 0);
    QVector<MCRInvItem> &getItems();

    bool getIsCreative() const;
    void setIsCreative(bool value);

    QString itemNamespacedID(const int index = 0);
    QString itemName(const int index         = 0);

    void setBackground(QString color = "#8B8B8B");

    bool getAcceptTag() const;
    void setAcceptTag(bool value);

    bool getAcceptMultiItems() const;
    void setAcceptMultiItems(bool value);

signals:
    void itemChanged();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected slots:
    void onCustomContextMenu(const QPoint &point);
    /*void onClicked(); */

private:
    QVector<MCRInvItem> items;
    bool isCreative = false;
    QPoint mousePressPos;
    bool isDragged        = false;
    bool itemHidden       = false;
    bool acceptTag        = true;
    bool acceptMultiItems = true;

    void startDrag(QMouseEvent *event);
    void hideItem();
    void showItem();
    QString toolTipText();
};

#endif /* MCRInvSlot_H */
