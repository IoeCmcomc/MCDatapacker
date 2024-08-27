#ifndef LOCATIONCONDITIONDIALOG_H
#define LOCATIONCONDITIONDIALOG_H

#include "basecondition.h"
#include "loottablecondition.h"
#include "gameinfomodel.h"
#include "datawidgetcontroller.h"

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

private:
    GameInfoModel m_biomeModel;
    GameInfoModel m_dimensionModel;
    GameInfoModel m_featureModel;
    GameInfoModel m_fluidModel;
    DataWidgetControllerRecord m_controller;
    Ui::LocationConditionDialog *ui;
    bool from_1_17 = false;

    void initBlockGroup();
    void initFluidGroup();

    static void setupStateTableFromJson(QTableWidget *table,
                                        const QJsonObject &json);

    static QJsonObject jsonFromStateTable(const QTableWidget *table);
};

#endif /* LOCATIONCONDITIONDIALOG_H */
