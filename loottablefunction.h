#ifndef LOOTTABLEFUNCTION_H
#define LOOTTABLEFUNCTION_H

#include "vieweventfilter.h"

#include <QTabWidget>
#include <QStandardItemModel>
#include <QHBoxLayout>

namespace Ui {
    class LootTableFunction;
}

class LootTableFunction : public QTabWidget
{
    Q_OBJECT

public:
    explicit LootTableFunction(QWidget *parent = nullptr);
    ~LootTableFunction();

    QJsonObject toJson() const;

private slots:
    void onTypeChanged(int index);
    void onAddCondition();
    void copyNBT_onAdded();
    void copyState_onAdded();
    void enchantRand_onAdded();
    void setAttr_onAdded();
    void entries_onAdded();
    void effectStew_onAdded();

private:
    Ui::LootTableFunction *ui;

    QStandardItemModel blocksModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel effectsModel;
    QStandardItemModel featuresModel;
    QStandardItemModel mapIconsModel;
    QStandardItemModel attributesModel;
    QHBoxLayout conditionsLayout;
    QHBoxLayout entriesLayout;
    const QStringList functTypes =
    { "apply_bonus",      "copy_nbt",            "copy_state",
      "enchant_randomly", "enchant_with_levels", "exploration_map",
      "fill_player_head", "limit_count",         "looting_enchant",
      "set_attributes",   "set_contents",        "set_count",
      "set_damage",       "set_loot_table",      "set_lore",
      "set_name",         "set_nbt",             "set_stew_effect",
      "copy_name",        "explosion_decay",     "furnace_smelt" };
    const QStringList formulaTypes = { "binomial_with_bonus_count",
                                       "uniform_bonus_count", "ore_drops" };
    const QStringList entityTargets = { "this", "killer", "killer_player",
                                        "block_entity" };
    const QStringList operationTypes = { "addition", "multiply_base",
                                         "multiply_total" };
    ViewEventFilter viewFilter;

    void initBlocksModel();
};

#endif /* LOOTTABLEFUNCTION_H */
