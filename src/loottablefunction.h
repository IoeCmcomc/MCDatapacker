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
    void fromJson(const QJsonObject &root);

protected:
    void changeEvent(QEvent *event) override;

private /*slots*/ :
    void onTypeChanged(int index);
    void onTabChanged(int index);
    void copyNBT_onAdded();
    void copyState_onAdded();
    void enchantRand_onAdded();
    void setAttr_onAdded();
    void effectStew_onAdded();
    void updateConditionsTab(int size);

private:
    enum Type {
        ApplyBonus,
        CopyNbt,
        CopyState,
        EnchantRandomly,
        EnchantWithLevels,
        ExplorationMap,
        FillPlayerHead,
        LimitCount,
        LootingEnchant,
        SetAttributes,
        SetBannerPattern,
        SetContents,
        SetCount,
        SetDamage,
        SetEnchantments,
        SetInstrument,
        SetLootTable,
        SetLore,
        SetName,
        SetNbt,
        SetPotion,
        SetStewEffect,
        CopyName,
        ExplosionDecay,
        FurnaceSmelt = ExplosionDecay,
    };

    QHBoxLayout conditionsLayout;
    QHBoxLayout entriesLayout;
    QStandardItemModel blocksModel;
    QStandardItemModel enchantmentsModel;
    QStandardItemModel effectsModel;
    QStandardItemModel featuresModel;
    QStandardItemModel mapIconsModel;
    QStandardItemModel attributesModel;
    QStandardItemModel blockEntityTypesModel;
    ViewEventFilter viewFilter;
    const QStringList functTypes =
    { "apply_bonus",      "copy_nbt",            "copy_state",
      "enchant_randomly", "enchant_with_levels", "exploration_map",
      "fill_player_head", "limit_count",         "looting_enchant",
      "set_attributes",   "set_banner_pattern",  "set_contents",
      "set_count",        "set_damage",          "set_enchantments",
      "set_instrument",
      "set_loot_table",   "set_lore",            "set_name",
      "set_nbt",          "set_potion",          "set_stew_effect",
      "copy_name",        "explosion_decay",     "furnace_smelt" };
    const QStringList formulaTypes = { "binomial_with_bonus_count",
                                       "uniform_bonus_count", "ore_drops" };
    const QStringList entityTargets = { "this", "killer", "killer_player",
                                        "block_entity" };
    const QStringList operationTypes = { "addition", "multiply_base",
                                         "multiply_total" };
    const QStringList nbtOperationTypes = { "append", "merge", "replace" };
    const QStringList slotTypes         = {
        "mainhand", "offhand", "head", "chest", "legs", "feet" };

    Ui::LootTableFunction *ui;

    void initBlocksModel();
    void initCondInterface();
    void initEntryInterface();
    void initBannerPatterns();
};

#endif /* LOOTTABLEFUNCTION_H */
