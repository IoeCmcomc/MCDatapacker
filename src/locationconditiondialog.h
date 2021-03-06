#ifndef LOCATIONCONDITIONDIALOG_H
#define LOCATIONCONDITIONDIALOG_H

#include "numericinput.h"
#include "basecondition.h"
#include "mcrpredcondition.h"

#include <QDialog>
#include <QVariant>

namespace Ui {
    class LocationConditionDialog;
}

class LocationConditionDialog : public QDialog, public BaseCondition
{
    Q_OBJECT
    friend class MCRPredCondition;

public:
    explicit LocationConditionDialog(QWidget *parent = nullptr);
    ~LocationConditionDialog();

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &value) override;

protected slots:
    void onAddedState();

private:
    Ui::LocationConditionDialog *ui;
    QStandardItemModel biomesModel;
    QStandardItemModel dimensionsModel;
    QStandardItemModel featuresModel;
    QStandardItemModel blocksModel;
    QStandardItemModel fluidsModel;

    void initBlockGroup();
    void initFluidGroup();

    static void setupStateTableFromJson(QTableWidget *table,
                                        const QJsonObject &json);
    static QJsonObject jsonFromStateTable(const QTableWidget *table);
};

#endif /* LOCATIONCONDITIONDIALOG_H */
