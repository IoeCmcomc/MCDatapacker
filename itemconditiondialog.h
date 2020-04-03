#ifndef ITEMCONDITIONDIALOG_H
#define ITEMCONDITIONDIALOG_H

#include "mcrpredcondition.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <QJsonObject>

namespace Ui {
    class ItemConditionDialog;
}

class ItemConditionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemConditionDialog(QWidget *parent = nullptr);
    ~ItemConditionDialog();

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &value);

protected slots:
    void onAddedEnchant();
    void onAddedStoredEnchant();
    void checkOK();

private:
    Ui::ItemConditionDialog *ui;
    QStandardItemModel itemsModel;
    QStandardItemModel potionsModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel itemEnchantModel;
    QStandardItemModel storedEnchantModel;
    const QString deleteableToolTip = "Right click this row to delete.";
    MCRPredCondition viewEventFilter;

    void initModelView(QStandardItemModel &model, QTableView *tableView);
};

#endif /* ITEMCONDITIONDIALOG_H */
