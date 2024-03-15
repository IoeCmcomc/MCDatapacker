#include "entityconditiondialog.h"
#include "ui_entityconditiondialog.h"

#include "modelfunctions.h"
#include "numberproviderdelegate.h"
#include "itemconditiondialog.h"
#include "locationconditiondialog.h"
#include "uberswitch.hpp"

#include "game.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QScrollArea>

static const QVector<QString> gamemodes = {
    "", "survival", "creative", "adventure", "spectator",
};

static const QVector<QString> catVariants = {
    "",       "all_black",       "black",            "british_shorthair",
    "calico", "jellie",          "persian",          "ragdoll",
    "red",    "siamese",         "tabby",            "tuxedo",
    "white",
};

static const QVector<QString> frogVariants = {
    "", "cold", "temperate", "warm",
};

EntityConditionDialog::EntityConditionDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::EntityConditionDialog) {
    ui->setupUi(this);

    m_entityModel.setInfo(QStringLiteral("entity"));
    ui->entityTypeCombo->setModel(&m_entityModel);
    m_effectModel.setInfo(QStringLiteral("effect"),
                          GameInfoModel::PrependPrefix);
    ui->effectCombo->setModel(&m_effectModel);
    m_statTypeModel.setInfo(QStringLiteral("stat_type"),
                            GameInfoModel::PrependPrefix);
    ui->statTypeCombo->setModel(&m_statTypeModel);

    m_advancementModel.setRegistry(QStringLiteral("advancement"),
                                   GameInfoModel::PrependPrefix);
    m_advancementModel.setDatapackCategory(QStringLiteral("advancements"));
    ui->advanEdit->setCompleter(m_advancementModel.createCompleter());

    m_recipeModel.setRegistry(QStringLiteral("recipe"),
                              GameInfoModel::PrependPrefix);
    m_recipeModel.setDatapackCategory(QStringLiteral("recipes"));
    ui->recipeEdit->setCompleter(m_recipeModel.createCompleter());

    ui->tabWidget->setTabVisible(2, Game::version() >= Game::v1_16);
    ui->tabWidget->setTabVisible(3, Game::version() >= Game::v1_17);
    ui->tabWidget->setTabVisible(4, Game::version() >= Game::v1_19);
    ui->tabWidget->setTabVisible(5, Game::version() >= Game::v1_19);

    ui->headPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->chestPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->legsPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->feetPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->mainhandPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->offhandPropBtn->assignDialogClass<ItemConditionDialog>();
    ui->locatPropBtn->assignDialogClass<LocationConditionDialog>();
    if (Game::version() >= Game::v1_16) {
        ui->vehicleBtn->assignDialogClass<EntityConditionDialog>();
        ui->targetEntityBtn->assignDialogClass<EntityConditionDialog>();
    } else {
        ui->vehicleBtn->hide();
        ui->targetEntityBtn->hide();
    }
    if (Game::version() >= Game::v1_17) {
        ui->passengerBtn->assignDialogClass<EntityConditionDialog>();
        ui->steppingOnBtn->assignDialogClass<LocationConditionDialog>();
        ui->lookingAtBtn->assignDialogClass<EntityConditionDialog>();
        ui->entityStruckBtn->assignDialogClass<EntityConditionDialog>();
    } else {
        ui->passengerBtn->hide();
        ui->steppingOnBtn->hide();
        ui->lookingAtBtn->hide();
        ui->lookingAtSep->hide();
    }

    initEffectsPage();
    initPlayerAdv();
    initPlayerRecipe();
    initPlayerStat();

    for (int i = 0; i < frogVariants.size(); ++i) {
        const QString &&fullId = QStringLiteral("minecraft:") + frogVariants.at(
            i);
        ui->frogVariantCombo->setItemData(i, fullId, Qt::UserRole + 1);
    }

    connect(ui->entityTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &EntityConditionDialog::onEntityTypeChanged);
}

EntityConditionDialog::~EntityConditionDialog() {
    delete ui;
}

QJsonObject EntityConditionDialog::toJson() const {
    QJsonObject root;

    if (ui->entityTypeCombo->currentIndex() != 0)
        root.insert(QStringLiteral("type"), ui->entityTypeCombo->currentData(
                        Qt::UserRole + 1).toJsonValue());
    if (!ui->teamEdit->text().isEmpty())
        root.insert(QStringLiteral("team"), ui->teamEdit->text());
    if (!ui->NBTEdit->toPlainText().isEmpty())
        root.insert(QStringLiteral("nbt"), ui->NBTEdit->toPlainText());
    QJsonObject distance;
    if (!ui->absDistanceInput->isCurrentlyUnset())
        distance.insert(QStringLiteral("absolute"),
                        ui->absDistanceInput->toJson());
    if (!ui->horizDistanceInput->isCurrentlyUnset())
        distance.insert(QStringLiteral("horizontal"),
                        ui->horizDistanceInput->toJson());
    if (!ui->xDistanceInput->isCurrentlyUnset())
        distance.insert(QStringLiteral("x"), ui->xDistanceInput->toJson());
    if (!ui->yDistanceInput->isCurrentlyUnset())
        distance.insert(QStringLiteral("y"), ui->yDistanceInput->toJson());
    if (!ui->zDistanceInput->isCurrentlyUnset())
        distance.insert(QStringLiteral("z"), ui->zDistanceInput->toJson());
    if (!distance.isEmpty())
        root.insert(QStringLiteral("distance"), distance);
    QJsonObject flags;
    ui->onFireCheck->insertToJsonObject(flags, QStringLiteral("is_on_fire"));
    ui->isSprintingCheck->insertToJsonObject(flags,
                                             QStringLiteral("is_sprinting"));
    ui->isSneakingCheck->insertToJsonObject(flags,
                                            QStringLiteral("is_sneaking"));
    ui->isSwimmingCheck->insertToJsonObject(flags,
                                            QStringLiteral("is_swimming"));
    ui->isBabyCheck->insertToJsonObject(flags, "is_baby");
    if (!flags.isEmpty())
        root.insert(QStringLiteral("flags"), flags);
    QJsonObject equipment;
    if (!ui->mainhandPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("mainhand"),
                         ui->mainhandPropBtn->getData());
    if (!ui->offhandPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("offhand"),
                         ui->offhandPropBtn->getData());
    if (!ui->headPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("head"), ui->headPropBtn->getData());
    if (!ui->chestPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("chest"), ui->chestPropBtn->getData());
    if (!ui->legsPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("legs"), ui->legsPropBtn->getData());
    if (!ui->feetPropBtn->getData().isEmpty())
        equipment.insert(QStringLiteral("feet"), ui->feetPropBtn->getData());
    if (!equipment.isEmpty())
        root.insert(QStringLiteral("equipment"), equipment);
    if (!ui->locatPropBtn->getData().isEmpty())
        root.insert(QStringLiteral("location"), ui->locatPropBtn->getData());
    if (!ui->vehicleBtn->getData().isEmpty()
        && (Game::version() >= Game::v1_16)) {
        root.insert(QStringLiteral("vehicle"), ui->vehicleBtn->getData());
    }
    if (Game::version() >= Game::v1_17) {
        if (!ui->passengerBtn->getData().isEmpty())
            root.insert(QStringLiteral("passenger"),
                        ui->passengerBtn->getData());
        if (!ui->steppingOnBtn->getData().isEmpty())
            root.insert(QStringLiteral("stepping_on"),
                        ui->steppingOnBtn->getData());
    }

    QJsonObject effects;
    for (int i = 0; i < entityEffectModel.rowCount(); ++i) {
        auto effect =
            entityEffectModel.item(i, 0)->data(Qt::UserRole + 1).toString();
        auto amplifier = entityEffectModel.item(i, 1)
                         ->data(ExtendedRole::NumberProviderRole).toJsonValue();
        auto duration =
            entityEffectModel.item(i, 2)
            ->data(ExtendedRole::NumberProviderRole).toJsonValue();
        auto ambient =
            entityEffectModel.item(i, 3)->data(Qt::DisplayRole).toBool();
        auto visible =
            entityEffectModel.item(i, 4)->data(Qt::DisplayRole).toBool();
        effects.insert(effect,
                       QJsonObject({ { QStringLiteral("amplifier"), amplifier },
                                       { QStringLiteral("duration"), duration },
                                       { QStringLiteral("ambient"), ambient },
                                       { QStringLiteral("visible"),
                                         visible } }));
    }
    if (!effects.isEmpty())
        root.insert(QStringLiteral("effects"), effects);

    QString &&entityId = ui->entityTypeCombo->currentData(
        Qt::UserRole + 1).toString();
    Glhp::removePrefix(entityId, "minecraft:"_QL1);
    const bool isNotSelected = ui->entityTypeCombo->currentIndex() == 0;

    if (isNotSelected || (entityId == QStringLiteral("player"))) {
        QJsonObject player;
        if (ui->gameModeCombo->currentIndex() != 0)
            player.insert(QStringLiteral("gamemode"),
                          gamemodes[ui->gameModeCombo->currentIndex()]);
        if (!ui->playerLevelInput->isCurrentlyUnset())
            player.insert(QStringLiteral("level"),
                          ui->playerLevelInput->toJson());
        if (Game::version() >= Game::v1_17) {
            if (!ui->lookingAtBtn->getData().isEmpty())
                player.insert(QStringLiteral("looking_at"),
                              ui->lookingAtBtn->getData());
        }

        QJsonObject advancements;
        for (int i = 0; i < playerAdvanModel.rowCount(); ++i) {
            auto advanm =
                playerAdvanModel.item(i, 0)->data(Qt::DisplayRole).toString();
            auto granted =
                playerAdvanModel.item(i, 1)->data(Qt::DisplayRole).toBool();
            advancements.insert(advanm, granted);
        }
        if (!advancements.isEmpty())
            player.insert(QStringLiteral("advancements"), advancements);

        QJsonObject recipes;
        for (int i = 0; i < playerRecipeModel.rowCount(); ++i) {
            auto recipe =
                playerRecipeModel.item(i, 0)->data(Qt::DisplayRole).toString();
            auto granted =
                playerRecipeModel.item(i, 1)->data(Qt::DisplayRole).toBool();
            recipes.insert(recipe, granted);
        }
        if (!recipes.isEmpty())
            player.insert(QStringLiteral("recipes"), recipes);

        QJsonArray stats;
        for (int i = 0; i < playerStatModel.rowCount(); ++i) {
            auto type =
                playerStatModel.item(i, 0)->data(Qt::UserRole + 1).toString();
            auto stat =
                playerStatModel.item(i, 1)->data(Qt::DisplayRole).toString();
            auto value = playerStatModel.item(i, 2)->data(
                ExtendedRole::NumberProviderRole).toJsonValue();
            stats.push_back(QJsonObject({ { QStringLiteral("type"), type },
                                            { QStringLiteral("stat"), stat },
                                            { QStringLiteral(
                                                  "value"), value } }));
        }
        if (!stats.isEmpty())
            player.insert(QStringLiteral("stats"), stats);
        if (!player.isEmpty()) {
            if (Game::version() >= Game::v1_19) {
                player.insert("type", "player");
                root.insert("type_specific", player);
            } else {
                root.insert(QStringLiteral("player"), player);
            }
        }
    }

    if ((isNotSelected || (entityId == QStringLiteral("cat")))
        && (ui->catVariantCombo->currentIndex() > 0)) {
        const auto &variant = catVariants[ui->catVariantCombo->currentIndex()];
        if (Game::version() >= Game::v1_19) {
            QJsonObject cat({ { "type", "cat" } });
            cat.insert("variant", QString("minecraft:" + variant));
            root.insert("type_specific", cat);
        } else {
            root.insert(QStringLiteral("catType"),
                        QStringLiteral("textures/entity/cat/%1.png").arg(
                            variant));
        }
    }

    if ((isNotSelected ||
         (entityId == QStringLiteral("fishing_bobber")))
        && (Game::version() >= Game::v1_16)) {
        QJsonObject fishingHook;
        ui->inOpenWaterCheck->insertToJsonObject(fishingHook,
                                                 QStringLiteral("in_open_water"));
        if (!fishingHook.isEmpty()) {
            if (Game::version() >= Game::v1_19) {
                fishingHook["type"]   = "fishing_hook";
                root["type_specific"] = fishingHook;
            } else {
                root["fishing_hook"] = fishingHook;
            }
        }
    }

    if ((isNotSelected ||
         (entityId == QStringLiteral("lightning_bolt")))
        && (Game::version() >= Game::v1_17)) {
        QJsonObject lightningBolt;
        if (!ui->blocksOnFireInput->isCurrentlyUnset()) {
            lightningBolt.insert("lightning_bolt",
                                 ui->blocksOnFireInput->toJson());
        }
        if (!ui->entityStruckBtn->getData().isEmpty())
            lightningBolt.insert(QStringLiteral("entity_struck"),
                                 ui->entityStruckBtn->getData());
        if (!lightningBolt.isEmpty()) {
            if (Game::version() >= Game::v1_19) {
                lightningBolt["type"] = "lightning";
                root["type_specific"] = lightningBolt;
            } else {
                root["lightning_bolt"] = lightningBolt;
            }
        }
    }

    if ((isNotSelected || (entityId == QStringLiteral("frog")))
        && (Game::version() >= Game::v1_19)
        && (ui->frogVariantCombo->currentIndex() != 0)) {
        QJsonObject frog{ { "type", "frog" } };
        frog.insert(QStringLiteral("variant"),
                    ui->frogVariantCombo->currentData(
                        Qt::UserRole + 1).toString());
        root["type_specific"] = frog;
    }

    if ((isNotSelected ||
         (entityId == QStringLiteral("slime") ||
          entityId == QStringLiteral("magma_cube")))
        && (Game::version() >= Game::v1_19)
        && !ui->slimeSizeInput->isCurrentlyUnset()) {
        QJsonObject slime{ { "type", "slime" } };
        slime["size"]         = ui->slimeSizeInput->toJson();
        root["type_specific"] = slime;
    }

    return root;
}

void EntityConditionDialog::playerFromJson(const QJsonObject &player) {
    if (player.contains("gamemode")) {
        const int index =
            gamemodes.indexOf(player[QStringLiteral("gamemode")].toString());
        if (index != -1) {
            ui->gameModeCombo->setCurrentIndex(index);
        }
    }
    if ((Game::version() >= Game::v1_17)
        && player.contains(QStringLiteral("looking_at"))) {
        ui->lookingAtBtn->setData(player[QStringLiteral(
                                             "looking_at")].toObject());
    }
    if (player.contains(QStringLiteral("level")))
        ui->playerLevelInput->fromJson(player[QStringLiteral("level")]);
    if (player.contains(QStringLiteral("advancements")))
        setupGrantedTableFromJson(player[QStringLiteral(
                                             "advancements")].toObject(),
                                  playerAdvanModel);
    if (player.contains(QStringLiteral("recipes")))
        setupGrantedTableFromJson(player[QStringLiteral(
                                             "recipes")].toObject(),
                                  playerRecipeModel);
    if (player.contains(QStringLiteral("stats"))) {
        auto stats = player[QStringLiteral("stats")].toArray();
        for (auto stat : stats) {
            QJsonObject statObj = stat.toObject();
            if (statObj.isEmpty())
                continue;
            QString statType = statObj[QStringLiteral("type")].toString();
            if (!statType.contains(QStringLiteral(":")))
                statType = QStringLiteral("minecraft:") + statType;
            auto indexes = m_statTypeModel.match(
                m_statTypeModel.index(0, 0), Qt::UserRole + 1, statType);
            if (indexes.isEmpty()) continue;
            auto *typeItem = new QStandardItem();
            typeItem->setData(statType);
            typeItem->setText(statType);
            typeItem->setEditable(false);
            auto *statItem = new QStandardItem();
            statItem->setText(statObj[QStringLiteral("stat")].toString());
            auto *valueItem = new QStandardItem();
            valueItem->setData(statObj.value(QStringLiteral("value")),
                               ExtendedRole::NumberProviderRole);

            playerStatModel.appendRow({ typeItem, statItem, valueItem });
        }
    }
}

void EntityConditionDialog::fromJson(const QJsonObject &value) {
    if (value.contains(QStringLiteral("type")))
        setComboValueFrom(ui->entityTypeCombo, value[QStringLiteral("type")]);
    if (value.contains(QStringLiteral("team")))
        ui->teamEdit->setText(value[QStringLiteral("team")].toString());
    if (value.contains(QStringLiteral("nbt")))
        ui->NBTEdit->setPlainText(value[QStringLiteral("nbt")].toString());
    if (value.contains(QStringLiteral("distance"))) {
        auto distance = value[QStringLiteral("distance")].toObject();
        if (distance.contains(QStringLiteral("absolute")))
            ui->absDistanceInput->fromJson(distance[QStringLiteral("absolute")]);
        if (distance.contains(QStringLiteral("horizontal")))
            ui->horizDistanceInput->fromJson(distance[QStringLiteral(
                                                          "horizontal")]);
        if (distance.contains(QStringLiteral("x")))
            ui->xDistanceInput->fromJson(distance[QStringLiteral("x")]);
        if (distance.contains(QStringLiteral("y")))
            ui->yDistanceInput->fromJson(distance[QStringLiteral("y")]);
        if (distance.contains(QStringLiteral("z")))
            ui->zDistanceInput->fromJson(distance[QStringLiteral("z")]);
    }
    if (value.contains(QStringLiteral("flags"))) {
        auto flags = value[QStringLiteral("flags")].toObject();
        ui->onFireCheck->setupFromJsonObject(flags,
                                             QStringLiteral("is_on_fire"));
        ui->isSprintingCheck->setupFromJsonObject(flags,
                                                  QStringLiteral("is_sprinting"));
        ui->isSneakingCheck->setupFromJsonObject(flags,
                                                 QStringLiteral("is_sneaking"));
        ui->isSwimmingCheck->setupFromJsonObject(flags,
                                                 QStringLiteral("is_swimming"));
        ui->isBabyCheck->setupFromJsonObject(flags, QStringLiteral("is_baby"));
    }
    if (value.contains(QStringLiteral("equipment"))) {
        auto equipment = value[QStringLiteral("equipment")].toObject();
        if (equipment.contains(QStringLiteral("mainhand")))
            ui->mainhandPropBtn->setData(equipment[QStringLiteral(
                                                       "mainhand")].toObject());
        if (equipment.contains(QStringLiteral("offhand")))
            ui->offhandPropBtn->setData(equipment[QStringLiteral(
                                                      "offhand")].toObject());
        if (equipment.contains(QStringLiteral("head")))
            ui->headPropBtn->setData(equipment[QStringLiteral(
                                                   "head")].toObject());
        if (equipment.contains(QStringLiteral("chest")))
            ui->chestPropBtn->setData(equipment[QStringLiteral(
                                                    "chest")].toObject());
        if (equipment.contains(QStringLiteral("legs")))
            ui->legsPropBtn->setData(equipment[QStringLiteral(
                                                   "legs")].toObject());
        if (equipment.contains(QStringLiteral("feet")))
            ui->feetPropBtn->setData(equipment[QStringLiteral(
                                                   "feet")].toObject());
    }
    if (value.contains(QStringLiteral("location")))
        ui->locatPropBtn->setData(value[QStringLiteral("location")].toObject());
    if (Game::version() >= Game::v1_16) {
        if (value.contains(QStringLiteral("vehicle")))
            ui->locatPropBtn->setData(value[QStringLiteral(
                                                "vehicle")].toObject());
        if (value.contains(QStringLiteral("targeted_entity")))
            ui->targetEntityBtn->setData(value[QStringLiteral(
                                                   "targeted_entity")].toObject());
    }
    if (Game::version() >= Game::v1_17) {
        if (value.contains(QStringLiteral("passenger")))
            ui->passengerBtn->setData(value[QStringLiteral(
                                                "passenger")].toObject());
        if (value.contains(QStringLiteral("stepping_on")))
            ui->steppingOnBtn->setData(value[QStringLiteral(
                                                 "stepping_on")].toObject());
    }

    if (value.contains(QStringLiteral("effects"))) {
        auto effects = value[QStringLiteral("effects")].toObject();
        for (auto &&effectID : effects.keys()) {
            if (!effectID.contains(QStringLiteral(":")))
                effectID = QStringLiteral("minecraft:") + effectID;
            auto indexes = m_effectModel.match(
                m_effectModel.index(0, 0), Qt::UserRole + 1, effectID);
            if (indexes.isEmpty()) continue;
            auto *effectItem = new QStandardItem();
            effectItem->setData(effectID);
            effectItem->setText(indexes[0].data(Qt::DisplayRole).toString());
            effectItem->setEditable(false);
            QJsonObject effect        = effects[effectID].toObject();
            auto       *amplifierItem = new QStandardItem();
            amplifierItem->setData(effect["amplifier"].toVariant(),
                                   ExtendedRole::NumberProviderRole);
            auto *durationItem = new QStandardItem();
            durationItem->setData(effect["duration"].toVariant(),
                                  ExtendedRole::NumberProviderRole);
            auto *ambientItem = new QStandardItem();
            ambientItem->setData(effect["ambient"].toBool(),
                                 Qt::DisplayRole);
            auto *visibleItem = new QStandardItem();
            visibleItem->setData(effect["visible"].toBool(),
                                 Qt::DisplayRole);

            entityEffectModel.appendRow({ effectItem, amplifierItem,
                                          durationItem,
                                          ambientItem, visibleItem });
        }
    }
    onEntityTypeChanged();
    if (Game::version() >= Game::v1_19) {
        if (value.contains("type_specific")) {
            const auto &typeSpecific = value["type_specific"].toObject();
            uswitch (typeSpecific["type"].toString()) {
                ucase ("player"_QL1): {
                    playerFromJson(typeSpecific);
                    break;
                }
                ucase ("cat"_QL1): {
                    auto &&variantStr =
                        typeSpecific[QStringLiteral("variant")].toString();
                    Glhp::removePrefix(variantStr, "minecraft:"_QL1);
                    for (int i = 1; i < catVariants.size(); ++i) {
                        if (variantStr == catVariants[i]) {
                            ui->catVariantCombo->setCurrentIndex(i);
                            break;
                        }
                    }
                    break;
                }
                ucase ("fishing_hook"_QL1): {
                    ui->inOpenWaterCheck->setupFromJsonObject(typeSpecific,
                                                              "in_open_water");
                    break;
                }
                ucase ("lightning"_QL1): {
                    if (typeSpecific.contains("blocks_set_on_fire")) {
                        ui->blocksOnFireInput->fromJson(typeSpecific[
                                                            "blocks_set_on_fire"]);
                    }
                    if (typeSpecific.contains("entity_struck")) {
                        ui->entityStruckBtn->setData(
                            typeSpecific["entity_struck"].toObject());
                    }
                    break;
                }
                ucase ("frog"_QL1): {
                    setComboValueFrom(ui->frogVariantCombo,
                                      typeSpecific[QStringLiteral("variant")]);
                    break;
                }
                ucase ("slime"_QL1): {
                    ui->slimeSizeInput->fromJson(
                        typeSpecific[QStringLiteral("size")]);
                    break;
                }
            }
        }
    } else {
        if (value.contains(QStringLiteral("player"))) {
            auto player = value[QStringLiteral("player")].toObject();
            playerFromJson(player);
        }
        if (value.contains(QStringLiteral("catType"))) {
            const auto &variantStr =
                value[QStringLiteral("catType")].toString();
            for (int i = 1; i < catVariants.size(); ++i) {
                if (variantStr ==
                    QStringLiteral("textures/entity/cat/%1.png").arg(catVariants
                                                                     [i])) {
                    ui->catVariantCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (value.contains(QStringLiteral("fishing_hook"))
            && (Game::version() >= Game::v1_16)) {
            const auto &fishingHook =
                value[QStringLiteral("fishing_hook")].toObject();
            ui->inOpenWaterCheck->setupFromJsonObject(fishingHook,
                                                      "in_open_water");
        }
        if (value.contains(QStringLiteral("lightning_bolt"))
            && (Game::version() >= Game::v1_17)) {
            const auto &lightningBolt =
                value[QStringLiteral("lightning_bolt")].toObject();
            if (lightningBolt.contains("blocks_set_on_fire")) {
                ui->blocksOnFireInput->fromJson(lightningBolt[
                                                    "blocks_set_on_fire"]);
            }
            if (lightningBolt.contains("entity_struck")) {
                ui->entityStruckBtn->setData(
                    lightningBolt["entity_struck"].toObject());
            }
        }
    }
}

void EntityConditionDialog::onAddedEntityEffect() {
    if ((ui->effectAmpInput->isCurrentlyUnset())
        || (ui->effectDuraInput->isCurrentlyUnset()))
        return;

    QString effectText = ui->effectCombo->currentText();
    if (!entityEffectModel.findItems(effectText).isEmpty())
        return;

    auto *effectItem = new QStandardItem(effectText);
    effectItem->setData(ui->effectCombo->currentData(Qt::UserRole + 1));
    effectItem->setEditable(false);
    auto *amplifierItem = new QStandardItem();
    amplifierItem->setData(ui->effectAmpInput->toJson(),
                           ExtendedRole::NumberProviderRole);
    auto *durationItem = new QStandardItem();
    durationItem->setData(ui->effectDuraInput->toJson(),
                          ExtendedRole::NumberProviderRole);
    auto *ambientItem = new QStandardItem();
    ambientItem->setData(ui->effectAmbientCheck->isChecked(), Qt::DisplayRole);
    auto *visibleItem = new QStandardItem();
    visibleItem->setData(ui->effectVisibleCheck->isChecked(), Qt::DisplayRole);

    entityEffectModel.appendRow({ effectItem, amplifierItem, durationItem,
                                  ambientItem, visibleItem });
}

void EntityConditionDialog::onAddedPlayerAdv() {
    QString advText = ui->advanEdit->text();

    if (!playerAdvanModel.findItems(advText).isEmpty())
        return;

    auto *advItem  = new QStandardItem(advText);
    auto *boolItem = new QStandardItem();
    boolItem->setData(ui->advanCheck->isChecked(), Qt::DisplayRole);

    playerAdvanModel.appendRow({ advItem, boolItem });
}

void EntityConditionDialog::onAddedPlayerRecipe() {
    QString recipeText = ui->recipeEdit->text();

    if (!playerRecipeModel.findItems(recipeText).isEmpty())
        return;

    auto *recipeItem = new QStandardItem(recipeText);
    auto *boolItem   = new QStandardItem();
    boolItem->setData(ui->recipeCheck->isChecked(), Qt::DisplayRole);

    playerRecipeModel.appendRow({ recipeItem, boolItem });
}

void EntityConditionDialog::onAddedPlayerStat() {
    if (ui->statEdit->text().isEmpty()
        || (ui->statValueInput->isCurrentlyUnset()))
        return;

    QStandardItem *typeItem =
        new QStandardItem(ui->statTypeCombo->currentText());
    typeItem->setData(ui->statTypeCombo->currentData(Qt::UserRole + 1));
    typeItem->setEditable(false);
    auto *statItem = new QStandardItem();
    statItem->setText(ui->statEdit->text());
    auto *valueItem = new QStandardItem();
    valueItem->setData(ui->statValueInput->toJson(),
                       ExtendedRole::NumberProviderRole);

    playerStatModel.appendRow({ typeItem, statItem, valueItem });
}

void EntityConditionDialog::onEntityTypeChanged() {
    /*qDebug() << "onEntityTypeChanged" << ui->player; */
    const QString &entityId = ui->entityTypeCombo->currentData(
        Qt::UserRole + 1).toString();
    const bool isNotSelected = ui->entityTypeCombo->currentIndex() == 0;

    ui->tabWidget->setTabEnabled(0, isNotSelected ||
                                 entityId ==
                                 QStringLiteral("minecraft:player"));
    ui->tabWidget->setTabEnabled(1, isNotSelected ||
                                 entityId == QStringLiteral("minecraft:cat"));

    if ((Game::version() >= Game::v1_16)) {
        ui->tabWidget->setTabEnabled(2, isNotSelected ||
                                     entityId ==
                                     QStringLiteral("minecraft:fishing_bobber"));
    }
    if ((Game::version() >= Game::v1_17)) {
        ui->tabWidget->setTabEnabled(3, isNotSelected ||
                                     entityId ==
                                     QStringLiteral("minecraft:lightning_bolt"));
    }
}

void EntityConditionDialog::initEffectsPage() {
    QStandardItem *effectItem    = new QStandardItem(tr("Effect"));
    QStandardItem *amplifierItem = new QStandardItem(tr("Amplifier"));
    QStandardItem *durationItem  = new QStandardItem(tr("Duration"));
    QStandardItem *ambientItem   = new QStandardItem(tr("Ambient"));
    QStandardItem *visibleItem   = new QStandardItem(tr("Visible"));

    auto *delegate = new NumberProviderDelegate(this);

    delegate->setInputModes(NumberProvider::ExactAndRange);
    delegate->setMinLimit(1);

    initModelView(entityEffectModel, ui->effectTableView,
                  { effectItem, amplifierItem, durationItem, ambientItem,
                    visibleItem }, viewFilter, delegate);
    ui->effectTableView->setColumnWidth(0, 150);
    ui->effectTableView->setColumnWidth(1, 150);
    ui->effectTableView->setColumnWidth(2, 150);

    connect(ui->effectAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedEntityEffect);
}

void EntityConditionDialog::initPlayerAdv() {
    QStandardItem *advItem     = new QStandardItem(tr("Advancement"));
    QStandardItem *grantedItem = new QStandardItem(tr("Granted"));

    initModelView(playerAdvanModel, ui->advanTable,
                  { advItem, grantedItem }, viewFilter);

    connect(ui->advanAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerAdv);
}

void EntityConditionDialog::initPlayerRecipe() {
    QStandardItem *recipeItem  = new QStandardItem(tr("Recipe"));
    QStandardItem *grantedItem = new QStandardItem(tr("Granted"));

    initModelView(playerRecipeModel, ui->recipeTable,
                  { recipeItem, grantedItem }, viewFilter);

    connect(ui->recipeAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerRecipe);
}

void EntityConditionDialog::initPlayerStat() {
    QStandardItem *typeItem  = new QStandardItem(tr("Type"));
    QStandardItem *statItem  = new QStandardItem(tr("Stat"));
    QStandardItem *valueItem = new QStandardItem(tr("Value"));

    auto *delegate = new NumberProviderDelegate(this);

    delegate->setInputModes(NumberProvider::Exact
                            | NumberProvider::Range);

    initModelView(playerStatModel, ui->statsTable,
                  { typeItem, statItem, valueItem }, viewFilter, delegate);

    connect(ui->statAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerStat);
}

void EntityConditionDialog::setupGrantedTableFromJson(const QJsonObject &json,
                                                      QStandardItemModel &model)
{
    const auto &&keys = json.keys();

    for (const auto &name : keys) {
        auto *nameItem = new QStandardItem(name);
        auto *boolItem = new QStandardItem();
        boolItem->setData(json[name].toBool(), Qt::DisplayRole);

        model.appendRow({ nameItem, boolItem });
    }
}
