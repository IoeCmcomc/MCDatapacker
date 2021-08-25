#ifndef BLOCKITEMSELECTORDIALOG_H
#define BLOCKITEMSELECTORDIALOG_H

#include "inventoryslot.h"
#include "inventoryitemfiltermodel.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QPushButton>


namespace Ui {
    class BlockItemSelectorDialog;
}

class BlockItemSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    using SelectCategory = InventorySlot::SelectCategory;
    using FilterModel    = InventoryItemFilterModel;

    explicit BlockItemSelectorDialog(QWidget *parent         = nullptr,
                                     SelectCategory category = SelectCategory::ObtainableItems);
    ~BlockItemSelectorDialog();

    QString getSelectedID();
    QVector<InventoryItem> getSelectedItems();

    void setAllowMultiple(bool value);

    void setCategory(const SelectCategory &category);

private slots:
    void checkOK();

private:
    InventoryItemFilterModel filterModel;
    QStandardItemModel model;
    Ui::BlockItemSelectorDialog *ui;
    QPushButton *selectButton = nullptr;
    SelectCategory m_category = SelectCategory::ObtainableItems;
    bool allowMultiple        = true;

    void setupListView();
};

#endif /* BLOCKITEMSELECTORDIALOG_H */
