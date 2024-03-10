#ifndef LOCATIONCONDITIONDIALOG_H
#define LOCATIONCONDITIONDIALOG_H

#include "basecondition.h"
#include "loottablecondition.h"
#include "gameinfomodel.h"

#include <QDialog>

namespace Ui {
    class LocationConditionDialog;
}

class LocationConditionDialog : public QDialog, public BaseCondition
{
    Q_OBJECT
    friend class LootTableCondition;

public:
    explicit LocationConditionDialog(QWidget *parent = nullptr);
    ~LocationConditionDialog();

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &value) override;

protected slots:
    void onAddedState();

private:
    Ui::LocationConditionDialog *ui;
    // QStandardItemModel biomesModel;
    GameInfoModel biomesModel;
    QStandardItemModel dimensionsModel;
    QStandardItemModel featuresModel;
    QStandardItemModel blocksModel;
    QStandardItemModel fluidsModel;
    bool from_1_17 = false;

    void initBlockGroup();
    void initFluidGroup();

    static void setupStateTableFromJson(QTableWidget *table,
                                        const QJsonObject &json);
    static QJsonObject jsonFromStateTable(const QTableWidget *table);
};

#endif /* LOCATIONCONDITIONDIALOG_H */
