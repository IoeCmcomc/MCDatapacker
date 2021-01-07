#ifndef MCRInvSlot_H
#define MCRInvSlot_H

#include "mcrinvitem.h"

#include <QFrame>
#include <QLabel>
#include <QBasicTimer>
#include <QTimerEvent>

class MCRInvSlot : public QFrame
{
    Q_OBJECT

public:
    explicit MCRInvSlot(QWidget *parent        = nullptr,
                        const MCRInvItem &item = MCRInvItem());

    void setItem(const QVector<MCRInvItem> &items);
    void setItem(const MCRInvItem &item);
    void appendItem(const MCRInvItem &item);
    void appendItems(const QVector<MCRInvItem> &items);
    void insertItem(const int index, const MCRInvItem &item);
    void removeItem(const int index);
    int removeItem(const MCRInvItem &item);
    void clearItems();
    MCRInvItem &getItem(const int index = 0);
    QVector<MCRInvItem> &getItems();

    bool getIsCreative() const;
    void setIsCreative(bool value);

    QString itemNamespacedID(const int index = 0);
    QString itemName(const int index         = 0);

    void setBackground(const QString &color = "#8B8B8B");

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
    void timerEvent(QTimerEvent *event) override;

protected slots:
    void onCustomContextMenu(const QPoint &point);
    void onItemChanged();

private:
    QVector<MCRInvItem> items;
    QPoint mousePressPos;
    QBasicTimer timer;
    bool isCreative       = false;
    bool isDragged        = false;
    bool itemHidden       = false;
    bool acceptTag        = true;
    bool acceptMultiItems = true;
    int curItemIndex      = -1;

    void startDrag(QMouseEvent *event);
    void hideItem();
    void showItem();
    QString toolTipText();
};

#endif /* MCRInvSlot_H */
