#ifndef MCRPREDCONDITION_H
#define MCRPREDCONDITION_H

#include <QFrame>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QJsonObject>

namespace Ui {
    class MCRPredCondition;
}

class MCRPredCondition : public QFrame
{
    Q_OBJECT

public:
    explicit MCRPredCondition(QWidget *parent = nullptr);
    ~MCRPredCondition();

    bool getIsModular() const;
    void setIsModular(bool value);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

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
    QStandardItemModel blockStateModel;
    QStandardItemModel entityScoresModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel tableBonusModel;
    QStandardItemModel toolEnchantModel;
    const QString deleteableToolTip = "Right click this row to delete.";
    QVBoxLayout nestedCondLayout;
    QVBoxLayout invertedCondLayout;
    QJsonObject matchTool_itemProp;

    void initBlockStatesPage();
    void initEntityScoresPage();
    void initInvertedCondPage();
    void initNestedCondPage();
    void initRandChancePage();
    void initTableBonusPage();
    void initToolEnchantPage();
};

#endif /* MCRPREDCONDITION_H */
