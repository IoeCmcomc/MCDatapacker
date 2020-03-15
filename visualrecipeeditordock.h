#ifndef VISUALRECIPEEDITORDOCK_H
#define VISUALRECIPEEDITORDOCK_H

#include "mcrinvslot.h"

#include <QDockWidget>
#include <QStandardItemModel>
#include <QListView>
/*#include <QVector> */

namespace Ui {
    class VisualRecipeEditorDock;
}

class VisualRecipeEditorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit VisualRecipeEditorDock(QWidget *parent = nullptr);
    ~VisualRecipeEditorDock();

    void writeRecipe();
    void readRecipe();
    void retranslate();

private slots:
    void onRecipeTabChanged(int index);
    /*void onRecipeChanged(); // Unused */

private:
    Ui::VisualRecipeEditorDock *ui;

    QVector<MCRInvSlot*> craftingSlots;
    bool isResizing    = false;
    int lastTabIndex   = 0;
    int lastStackIndex = 0;

    void setupCustomTab();
    QJsonObject genCraftingJson(QJsonObject root);
    QJsonObject genSmeltingJson(QJsonObject root);
    QJsonObject genStonecuttingJson(QJsonObject root);
    void readCraftingJson(const QJsonObject &root);
    void readSmeltingJson(const QJsonObject &root);
    void readStonecuttingJson(const QJsonObject &root);
};

QJsonValue ingredientsToJson(const QVector<MCRInvItem> &items);
QVector<MCRInvItem> JsonToIngredients(const QJsonValue &keyVal);

#endif /* VISUALRECIPEEDITORDOCK_H */
