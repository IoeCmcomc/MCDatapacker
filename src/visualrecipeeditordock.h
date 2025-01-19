#ifndef VISUALRECIPEEDITORDOCK_H
#define VISUALRECIPEEDITORDOCK_H

#include "inventoryslot.h"

#include <QDockWidget>
#include <QStandardItemModel>


namespace Ui {
    class VisualRecipeEditorDock;
}

class VisualRecipeEditorDock : public QDockWidget {
    Q_OBJECT

public:
    explicit VisualRecipeEditorDock(QWidget *parent = nullptr);
    ~VisualRecipeEditorDock();

    void writeRecipe();
    void readRecipe();
    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onRecipeTabChanged(int index);
    /*void onRecipeChanged(); // Unused */

private:
    Ui::VisualRecipeEditorDock *ui;

    QStandardItemModel m_craftCategories;
    QStandardItemModel m_smeltCategories;
    QVector<InventorySlot *> craftingSlots;
    int lastTabIndex   = 0;
    int lastStackIndex = 0;
    bool isResizing    = false;

    void setupCustomTab();
    void setupCategoryCombo();
    QJsonObject genCraftingJson(QJsonObject root);
    QJsonObject genSmeltingJson(QJsonObject root);
    QJsonObject genStonecuttingJson(QJsonObject root);
    QJsonObject genSmithingJson(QJsonObject root);

    void readCraftingJson(const QJsonObject &root);
    void readSmeltingJson(const QJsonObject &root);
    void readStonecuttingJson(const QJsonObject &root);
    void readSmithingJson(const QJsonObject &root);

    QVector<InventoryItem> JsonToIngredients(const QJsonValue &keyVal);
    void tryWriteComponents(QJsonObject &obj, InventorySlot *slot);
    void tryReadComponents(const QJsonObject &obj, InventorySlot *slot);
};

#endif /* VISUALRECIPEEDITORDOCK_H */
