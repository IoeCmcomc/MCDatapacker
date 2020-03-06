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

    void setItem(MCRInvItem item, bool emitSignal = false);
    void removeItem(bool emitSignal               = false);
    MCRInvItem getItem() const;
    bool getIsCreative() const;
    void setIsCreative(bool value);
    QString itemNamespacedID();
    QString itemName();

    void setBackground(QString color = "#8B8B8B");

    bool getAcceptTag() const;
    void setAcceptTag(bool value);

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
    MCRInvItem item;
    bool isCreative = false;
    QPoint mousePressPos;
    bool isDragged  = false;
    bool itemHidden = false;
    bool acceptTag  = true;

    void startDrag(QMouseEvent *event);
    void hideItem();
    void showItem();
};

#endif /* MCRInvSlot_H */
