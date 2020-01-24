#ifndef VISUALRECIPEEDITORDOCK_H
#define VISUALRECIPEEDITORDOCK_H

#include "mcrinvslot.h"

#include <QDockWidget>
//#include <QVector>

namespace Ui {
class VisualRecipeEditorDock;
}

class VisualRecipeEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit VisualRecipeEditorDock(QWidget *parent = nullptr);
    ~VisualRecipeEditorDock();

private slots:
    void onRecipeChanged();

private:
    Ui::VisualRecipeEditorDock *ui;

    QVector<MCRInvSlot*> CraftingSlots;

    void setupItemList();

};

#endif // VISUALRECIPEEDITORDOCK_H
