#ifndef LOCATIONCONDITIONDIALOG_H
#define LOCATIONCONDITIONDIALOG_H

#include "extendednumericinput.h"
#include "basecondition.h"

#include <QDialog>
#include <QVariant>

namespace Ui {
    class LocationConditionDialog;
}

class LocationConditionDialog : public QDialog, public BaseCondition
{
    Q_OBJECT

public:
    explicit LocationConditionDialog(QWidget *parent = nullptr);
    ~LocationConditionDialog();

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &value);

protected slots:
    void onAddedBlockState();
    void onAddedFluidState();

private:
    Ui::LocationConditionDialog *ui;
    QStandardItemModel biomesModel;
    QStandardItemModel dimensionsModel;
    QStandardItemModel featuresModel;
    QStandardItemModel blocksModel;
    QStandardItemModel blockStatesModel;
    QStandardItemModel fluidsModel;
    QStandardItemModel fluidStatesModel;

    void initBlockGroup();
    void initFluidGroup();
    void setupTableFromJson(QStandardItemModel &model, const QJsonObject &json);
};

#endif /* LOCATIONCONDITIONDIALOG_H */
