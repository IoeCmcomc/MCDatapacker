#ifndef BLOCKITEMSELECTORDIALOG_H
#define BLOCKITEMSELECTORDIALOG_H

#include "mcrinvitem.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QPushButton>
#include "mcritemsortfilterproxymodel.h"

namespace Ui {
    class BlockItemSelectorDialog;
}

class BlockItemSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlockItemSelectorDialog(QWidget *parent = nullptr);
    ~BlockItemSelectorDialog();

    QString getSelectedID();
    QVector<MCRInvItem> getSelectedItems();


    bool getAllowMultiItems() const;
    void setAllowMultiItems(bool value);

protected slots:
    void checkOK();

private:
    Ui::BlockItemSelectorDialog *ui;
    QStandardItemModel model;
    MCRItemSortFilterProxyModel filterModel;
    QPushButton *selectButton;
    bool allowMultiItems = true;

    void setupListView();
};

#endif /* BLOCKITEMSELECTORDIALOG_H */
