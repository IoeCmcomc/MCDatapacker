#ifndef ITEMCONDITIONDIALOG_H
#define ITEMCONDITIONDIALOG_H

#include "basecondition.h"

#include <QDialog>
#include <QJsonArray>

namespace Ui {
    class ItemConditionDialog;
}

class ItemConditionDialog : public QDialog, public BaseCondition
{
    Q_OBJECT

public:
    explicit ItemConditionDialog(QWidget *parent = nullptr);
    ~ItemConditionDialog();

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &value) override;

protected slots:
    void onAddedEnchant();
    void onAddedStoredEnchant();

private:
    Ui::ItemConditionDialog *ui;
    QStandardItemModel itemsModel;
    QStandardItemModel potionsModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel itemEnchantModel;
    QStandardItemModel storedEnchantModel;

    using BaseCondition::initModelView;
    void initModelView(QStandardItemModel &model, QTableView *tableView);
    void tableFromJson(const QJsonArray &jsonArr, QStandardItemModel &model);
};

#endif /* ITEMCONDITIONDIALOG_H */
