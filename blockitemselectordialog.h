#ifndef BLOCKITEMSELECTORDIALOG_H
#define BLOCKITEMSELECTORDIALOG_H

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

protected slots:
    void checkOK();

private:
    Ui::BlockItemSelectorDialog *ui;
    QStandardItemModel *model = new QStandardItemModel(this);
    MCRItemSortFilterProxyModel filterModel;
    QPushButton *selectButton;

    void setupTreeView();
};

#endif // BLOCKITEMSELECTORDIALOG_H
