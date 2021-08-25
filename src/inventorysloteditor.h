#ifndef INVENTORYSLOTEDITOR_H
#define INVENTORYSLOTEDITOR_H

#include "inventoryitem.h"
#include "inventoryslot.h"

#include <QWidget>
#include <QFrame>
#include <QVector>
#include <QStandardItemModel>

namespace Ui {
    class InventorySlotEditor;
}

class InventorySlotEditor : public QFrame
{
    Q_OBJECT

public:
    explicit InventorySlotEditor(InventorySlot *parent = nullptr);
    ~InventorySlotEditor();

    void show();

protected:
    void mousePressEvent(QMouseEvent *event) override;

protected slots:
    void onNewItem();
    void onNewItemTag();
    void checkRemove();
    void onRemoveItem();
    void close();

private:
    QStandardItemModel model;
    QPoint initPos;
    Ui::InventorySlotEditor *ui;
    InventorySlot *slot = nullptr;

    void appendItem(const InventoryItem &invItem);
};

#endif /* INVENTORYSLOTEDITOR_H */
