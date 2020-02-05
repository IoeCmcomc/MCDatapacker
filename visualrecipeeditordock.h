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
    void onRecipeTabChanged(int index);
    void onRecipeChanged();

private:
    Ui::VisualRecipeEditorDock *ui;

    QVector<MCRInvSlot*> CraftingSlots;
    int lastTabIndex = 0;
    int lastStackIndex = 0;

    void setupItemList();
    void setupTabBar();
    QJsonObject genCraftingJson(QJsonObject root);
    QJsonObject genSmeltingJson(QJsonObject root);
    QJsonObject genStonecuttingJson(QJsonObject root);

};

#endif // VISUALRECIPEEDITORDOCK_H
