#ifndef MCRINVSLOTEDITOR_H
#define MCRINVSLOTEDITOR_H

#include "mcrinvitem.h"
#include "mcrinvslot.h"

#include <QWidget>
#include <QFrame>
#include <QVector>
#include <QStandardItemModel>

namespace Ui {
    class MCRInvSlotEditor;
}

class MCRInvSlotEditor : public QFrame
{
    Q_OBJECT

public:
    explicit MCRInvSlotEditor(MCRInvSlot *parent = nullptr);
    ~MCRInvSlotEditor();

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
    Ui::MCRInvSlotEditor *ui;
    MCRInvSlot *slot = nullptr;
    QStandardItemModel model;
    QPoint initPos;

    void appendItem(const MCRInvItem &invItem);
};

#endif /* MCRINVSLOTEDITOR_H */
