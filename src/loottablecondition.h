#ifndef LOOTTABLECONDITION_H
#define LOOTTABLECONDITION_H

#include "basecondition.h"
#include "datawidgetcontroller.h"
#include "gameinfomodel.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QDir>

#include <mutex>


namespace Ui {
    class LootTableCondition;
}

class LootTableCondition : public QTabWidget, public BaseCondition {
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
    void showEvent(QShowEvent *event) override;

private slots:
    void blockStates_onAdded();
    void entityScores_onAdded();
    void tableBonus_onAdded();
    void toolEnchant_onAdded();
    void onTypeChanged(const int i);
    void onTabChanged(const int i);
    void onCurDirChanged(const QDir &dir);
    void updateConditionsTab(int size);

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
    GameInfoModel m_conditionModel;
    GameInfoModel m_damageTagModel;
    GameInfoModel m_enchantmentModel;
    QStandardItemModel tableBonusModel;
    const QStringList condTypes = {
        "block_state_property", "damage_source_properties", "entity_properties",
        "entity_scores",        "killed_by_player",         "location_check",
        "alternative",          "match_tool",               "random_chance",
        "reference",            "survives_explosion",       "table_bonus",
        "time_check",           "tool_enchantment",         "weather_check",
        "value_check",          };
    const QStringList entityTargets = { "this", "killer", "killer_player" };

    DataWidgetControllerRecord m_timeCtrl;
    std::once_flag m_fullyInitialized;

    void reset(int index);
    void clearModelExceptHeaders(QStandardItemModel &model);

    void init();
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
