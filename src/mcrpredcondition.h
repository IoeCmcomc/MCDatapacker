#ifndef MCRPREDCONDITION_H
#define MCRPREDCONDITION_H

#include "basecondition.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QFileSystemWatcher>
#include <QDir>

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

    QJsonObject toJson() const override;
    inline void fromJson(const QJsonObject &value) override {
        fromJson(value, false);
    };
    void fromJson(const QJsonObject &root, bool redirected);

    void setDepth(int value);

    void resetAll();

protected:
    void changeEvent(QEvent *) override;

protected slots:
    void blockStates_onAdded();
    void entityScores_onAdded();
    void nested_onAdded();
    void setupRefCombo();
    void tableBonus_onAdded();
    void toolEnchant_onAdded();
    void onTypeChanged(const int &i);
    void onCurDirChanged(const QDir &dir);

private:
    Ui::MCRPredCondition *ui;
    bool isModular = true;
    int depth      = 0;
    QStandardItemModel blocksModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel condRefsModel;
    QStandardItemModel tableBonusModel;
    QVBoxLayout nestedCondLayout;
    QVBoxLayout invertedCondLayout;
    const QStringList condTypes =
    { "block_state_property", "damage_source_properties", "entity_properties",
      "entity_scores",        "inverted",                 "killed_by_player",
      "location_check",       "alternative",              "match_tool",
      "random_chance",        "reference",                "survives_explosion",
      "table_bonus",          "time_check",               "tool_enchantment",
      "weather_check" };
    const QStringList entityTargets = { "this", "killer", "killer_player" };
    QFileSystemWatcher predRefWatcher;

    void reset(int index);
    void clearModelExceptHeaders(QStandardItemModel &model);

    void initBlockStatesPage();
    void initEntityScoresPage();
    void initInvertedCondPage();
    void initNestedCondPage();
    void initRandChancePage();
    void initTableBonusPage();
    void initToolEnchantPage();

    void addInvertCondition(QJsonObject &json) const;
    void simplifyCondition(QVariantMap &condMap, int depth = 0) const;
};

#endif /* MCRPREDCONDITION_H */
