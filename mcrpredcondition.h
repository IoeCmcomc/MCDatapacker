#ifndef MCRPREDCONDITION_H
#define MCRPREDCONDITION_H

#include "basecondition.h"

#include <QFrame>
#include <QStandardItemModel>
#include <QVBoxLayout>

namespace Ui {
    class MCRPredCondition;
}

class MCRPredCondition : public QFrame, public BaseCondition
{
    Q_OBJECT

public:
    explicit MCRPredCondition(QWidget *parent = nullptr);
    ~MCRPredCondition();

    bool getIsModular() const;
    void setIsModular(bool value);

protected slots:
    void blockStates_onAdded();
    void entityScores_onAdded();
    void nested_onAdded();
    void tableBonus_onAdded();
    void toolEnchant_onAdded();
    void onTypeChanged(const int &i);

private:
    Ui::MCRPredCondition *ui;
    bool isModular = true;
    QStandardItemModel blocksModel;
    QStandardItemModel blockStatesModel;
    QStandardItemModel entityScoresModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel tableBonusModel;
    QStandardItemModel toolEnchantModel;
    QVBoxLayout nestedCondLayout;
    QVBoxLayout invertedCondLayout;
    QJsonObject damageSrc_entityProp;
    QJsonObject matchTool_itemProp;
    QJsonObject location_locatProp;

    void initBlockStatesPage();
    void initEntityScoresPage();
    void initInvertedCondPage();
    void initNestedCondPage();
    void initRandChancePage();
    void initTableBonusPage();
    void initToolEnchantPage();
};

#endif /* MCRPREDCONDITION_H */
