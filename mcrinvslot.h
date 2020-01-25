#ifndef MCRInvSlot_H
#define MCRInvSlot_H

#include "mcrinvitem.h"

#include <QFrame>
#include <QLabel>
#include <QGridLayout>

class MCRInvSlot : public QFrame
{
    Q_OBJECT

public:
    explicit MCRInvSlot(QWidget *parent = nullptr, MCRInvItem *item = nullptr);

    void setItem(MCRInvItem *item, bool emitSignal = false);
    void removeItem(bool emitSignal = false);
    MCRInvItem* getItem();

    void setBackground(QString color = "#8B8B8B");

    bool isCreative = false;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void itemChanged();

private:
    MCRInvItem *item = nullptr;
};

#endif // MCRInvSlot_H
