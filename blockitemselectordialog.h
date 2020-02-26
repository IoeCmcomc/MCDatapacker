#ifndef BLOCKITEMSELECTORDIALOG_H
#define BLOCKITEMSELECTORDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QPushButton>
#include <QSortFilterProxyModel>

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
    void onListViewSearch(const QString &input);
    void checkOK();

private:
    Ui::BlockItemSelectorDialog *ui;
    QStandardItemModel *model = new QStandardItemModel();
    QSortFilterProxyModel filterModel;
    QPushButton *selectButton;

    void setupTreeView();
};

#endif // BLOCKITEMSELECTORDIALOG_H
