#ifndef LOCATIONCONDITIONDIALOG_H
#define LOCATIONCONDITIONDIALOG_H

#include "extendednumericinput.h"
#include "vieweventfilter.h"

#include <QComboBox>
#include <QDialog>
#include <QStandardItemModel>
#include <QVariant>

namespace Ui {
    class LocationConditionDialog;
}

class LocationConditionDialog : public QDialog
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
    const QString deletiveToolTip = "Right click this row to delete.";
    ViewEventFilter viewFilter;

    void initNumericInput(ExtendedNumericInput *input, const int &min,
                          const int &max);
    void initComboModelView(const QString &infoType,
                            QStandardItemModel &model, QComboBox *combo,
                            bool optional = true);
    void initBlockGroup();
    void initFluidGroup();

    void setupComboFrom(QComboBox *combo, const QVariant &vari,
                        int role = Qt:: UserRole + 1);
};

#endif /* LOCATIONCONDITIONDIALOG_H */
