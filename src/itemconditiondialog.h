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
    bool from_1_17;

    void initTable(QTableWidget *table);
    void tableFromJson(const QJsonArray &jsonArr, QTableWidget *table);
};

#endif /* ITEMCONDITIONDIALOG_H */
