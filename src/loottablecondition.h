#ifndef LOOTTABLECONDITION_H
#define LOOTTABLECONDITION_H

#include "basecondition.h"
#include "datawidgetcontroller.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QFileSystemWatcher>
#include <QDir>

namespace Ui {
    class LootTableCondition;
}

class LootTableCondition : public QFrame, public BaseCondition
{
    Q_OBJECT

public:
    explicit LootTableCondition(QWidget *parent = nullptr);
    ~LootTableCondition();

    QJsonObject toJson() const override;
    inline void fromJson(const QJsonObject &value) override {
        fromJson(value, false);
    };
    void fromJson(const QJsonObject &root, bool redirected);

    void setDepth(int value);

    void resetAll();

protected:
    void changeEvent(QEvent *) override;

private slots:
    void blockStates_onAdded();
    void entityScores_onAdded();
    void setupRefCombo();
    void tableBonus_onAdded();
    void toolEnchant_onAdded();
    void onTypeChanged(const int &i);
    void onCurDirChanged(const QDir &dir);

private:
    enum class NestedMode {
        Invalid,
        SingleInvert,
        Alternative,
        AnyOf,
        AllOf,
    };

    Ui::LootTableCondition *ui;
    int depth = 0;
    QStandardItemModel blocksModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel condRefsModel;
    QStandardItemModel tableBonusModel;
    const QStringList condTypes = {
        "block_state_property", "damage_source_properties", "entity_properties",
        "entity_scores",        "killed_by_player",         "location_check",
        "alternative",          "match_tool",               "random_chance",
        "reference",            "survives_explosion",       "table_bonus",
        "time_check",           "tool_enchantment",         "weather_check",
        "value_check",          };
    const QStringList entityTargets = { "this", "killer", "killer_player" };
    QFileSystemWatcher predRefWatcher;

    DataWidgetControllerRecord m_timeCtrl;

    void reset(int index);
    void clearModelExceptHeaders(QStandardItemModel &model);

    void initBlockStatesPage();
    void initDamageSrcPage();
    void initEntityScoresPage();
    void initNestedCondPage();
    void initRandChancePage();
    void initTableBonusPage();
    void initToolEnchantPage();

    void addInvertCondition(QJsonObject &json) const;
    void simplifyCondition(QVariantMap &condMap, int depth = 0) const;
};

#endif /* LOOTTABLECONDITION_H */
