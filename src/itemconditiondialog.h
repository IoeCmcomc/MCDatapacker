#ifndef ITEMCONDITIONDIALOG_H
#define ITEMCONDITIONDIALOG_H

#include "basecondition.h"
#include "datawidgetcontroller.h"

#include <QDialog>
#include <QJsonArray>

class ExtendedTableWidget;

namespace Ui {
    class ItemConditionDialog;
}

class ItemConditionDialog : public QDialog, public BaseCondition {
    Q_OBJECT

public:
    explicit ItemConditionDialog(QWidget *parent = nullptr);
    ~ItemConditionDialog();

    QJsonObject toJson() const final;
    void fromJson(const QJsonObject &value) final;

private:
    Ui::ItemConditionDialog *ui;
    DataWidgetControllerRecord m_controller;
    QStandardItemModel itemsModel;
    QStandardItemModel potionsModel;
    QStandardItemModel enchantmentsModel;

    void initTable(ExtendedTableWidget *table);
};

#endif /* ITEMCONDITIONDIALOG_H */
