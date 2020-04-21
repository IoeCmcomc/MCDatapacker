#include "entityconditiondialog.h"
#include "ui_entityconditiondialog.h"

#include "extendeddelegate.h"

#include <QDebug>
#include <QScrollArea>

EntityConditionDialog::EntityConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::EntityConditionDialog) {
    ui->setupUi(this);
    auto typeFlag = NumericInput::Exact
                    | NumericInput::Range;
    ui->absDistanceInput->setTypes(typeFlag);
    ui->horizDistanceInput->setTypes(typeFlag);
    ui->xDistanceInput->setTypes(typeFlag);
    ui->yDistanceInput->setTypes(typeFlag);
    ui->zDistanceInput->setTypes(typeFlag);
    ui->effectAmpInput->setTypes(typeFlag);
    ui->effectDuraInput->setTypes(typeFlag);
    ui->playerLevelInput->setTypes(typeFlag);
    ui->statValueInput->setTypes(typeFlag);

    ui->absDistanceInput->setGeneralMinimum(-999999999);
    ui->horizDistanceInput->setGeneralMinimum(-999999999);
    ui->xDistanceInput->setGeneralMinimum(-999999999);
    ui->yDistanceInput->setGeneralMinimum(-999999999);
    ui->zDistanceInput->setGeneralMinimum(-999999999);

    initComboModelView(QStringLiteral("entity"),
                       entityModel,
                       ui->entityTypeCombo);
    initComboModelView(QStringLiteral("effect"),
                       effectModel,
                       ui->effectCombo,
                       false);
    initComboModelView(QStringLiteral("stat_type"),
                       statTypeModel,
                       ui->statTypeCombo,
                       false);

    ui->headPropBtn->setDialogType(DDBType::ItemCond);
    ui->chestPropBtn->setDialogType(DDBType::ItemCond);
    ui->legsPropBtn->setDialogType(DDBType::ItemCond);
    ui->feetPropBtn->setDialogType(DDBType::ItemCond);
    ui->mainhandPropBtn->setDialogType(DDBType::ItemCond);
    ui->offhandPropBtn->setDialogType(DDBType::ItemCond);
    ui->locatPropBtn->setDialogType(DDBType::LocationCond);

    initEffectsPage();
    initPlayerAdv();
    initPlayerRecipe();
    initPlayerStat();

    connect(ui->entityTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &EntityConditionDialog::onEntityTypeChanged);
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
    if (!ui->NBTEdit->text().isEmpty())
        root.insert(QStringLiteral("nbt"), ui->NBTEdit->text());
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
        equipment.insert(QStringLiteral("feet"), ui->feetPropBtn->getData()); ;
    if (!equipment.isEmpty())
        root.insert(QStringLiteral("equipment"), equipment);
    if (!ui->locatPropBtn->getData().isEmpty())
        root.insert(QStringLiteral("location"), ui->locatPropBtn->getData());

    QJsonObject effects;
    for (int i = 0; i < entityEffectModel.rowCount(); ++i) {
        auto effect =
            entityEffectModel.item(i, 0)->data(Qt::UserRole + 1).toString();
        auto amplifier =
            entityEffectModel.item(i, 1)->data(Qt::DisplayRole).toJsonValue();
        auto duration =
            entityEffectModel.item(i, 2)->data(Qt::DisplayRole).toJsonValue();
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

    if ((ui->entityTypeCombo->currentIndex() == 0)
        || (ui->entityTypeCombo->currentData(Qt::UserRole + 1).toString() ==
            QStringLiteral("minecraft:player"))) {
        QJsonObject player;
        if (ui->gameModeCombo->currentIndex() != 0)
            player.insert(QStringLiteral("gamemode"),
                          ui->gameModeCombo->currentText().toLower());
        if (!ui->playerLevelInput->isCurrentlyUnset())
            player.insert(QStringLiteral("level"),
                          ui->playerLevelInput->toJson());

        QJsonObject advancements;
        for (int i = 0; i < playerAdvanmModel.rowCount(); ++i) {
            auto advanm =
                playerAdvanmModel.item(i, 0)->data(Qt::DisplayRole).toString();
            auto granted =
                playerAdvanmModel.item(i, 1)->data(Qt::DisplayRole).toBool();
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
            auto value =
                playerStatModel.item(i, 2)->data(Qt::DisplayRole).toJsonValue();
            stats.push_back(QJsonObject({ { QStringLiteral("type"), type },
                                            { QStringLiteral("stat"), stat },
                                            { QStringLiteral(
                                                  "value"), value } }));
        }
        if (!stats.isEmpty())
            player.insert(QStringLiteral("stats"), stats);
        if (!player.isEmpty())
            root.insert(QStringLiteral("player"), player);
    }
    ;

    return root;
}

void EntityConditionDialog::fromJson(const QJsonObject &value) {
    if (value.contains(QStringLiteral("type")))
        setupComboFrom(ui->entityTypeCombo, value[QStringLiteral("type")]);
    if (value.contains(QStringLiteral("team")))
        ui->teamEdit->setText(value[QStringLiteral("team")].toString());
    if (value.contains(QStringLiteral("nbt")))
        ui->NBTEdit->setText(value[QStringLiteral("nbt")].toString());
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
    if (value.contains(QStringLiteral("effects"))) {
        auto effects = value[QStringLiteral("effects")].toObject();
        for (auto effectID : effects.keys()) {
            if (!effectID.contains(QStringLiteral(":")))
                effectID = QStringLiteral("minecraft:") + effectID;
            auto indexes = effectModel.match(
                effectModel.index(0, 0), Qt::UserRole + 1, effectID);
            if (indexes.isEmpty()) continue;
            QStandardItem *effectItem = new QStandardItem();
            effectItem->setData(effectID);
            effectItem->setText(indexes[0].data(Qt::DisplayRole).toString());
            effectItem->setEditable(false);
            effectItem->setToolTip(deletiveToolTip);
            QJsonObject    effect        = effects[effectID].toObject();
            QStandardItem *amplifierItem = new QStandardItem();
            amplifierItem->setData(effect["amplifier"].toVariant(),
                                   Qt::DisplayRole);
            amplifierItem->setToolTip(deletiveToolTip);
            QStandardItem *durationItem = new QStandardItem();
            durationItem->setData(effect["duration"].toVariant(),
                                  Qt::DisplayRole);
            durationItem->setToolTip(deletiveToolTip);
            QStandardItem *ambientItem = new QStandardItem();
            ambientItem->setData(effect["ambient"].toBool(),
                                 Qt::DisplayRole);
            ambientItem->setToolTip(deletiveToolTip);
            QStandardItem *visibleItem = new QStandardItem();
            visibleItem->setData(effect["visible"].toBool(),
                                 Qt::DisplayRole);
            visibleItem->setToolTip(deletiveToolTip);

            entityEffectModel.appendRow({ effectItem, amplifierItem,
                                          durationItem,
                                          ambientItem, visibleItem });
        }
    }
    onEntityTypeChanged();
    if (value.contains(QStringLiteral("player"))) {
        auto player = value[QStringLiteral("player")].toObject();
        if (player.contains("gamemode")) {
            auto gamemode = player[QStringLiteral("gamemode")].toString();
            gamemode[0].toUpper();
            setupComboFrom(ui->gameModeCombo, gamemode);
        }
        if (player.contains(QStringLiteral("level")))
            ui->playerLevelInput->fromJson(player[QStringLiteral("level")]);
        if (player.contains(QStringLiteral("advancements")))
            setupGrantedTableFromJson(player[QStringLiteral(
                                                 "advancements")].toObject(),
                                      playerAdvanmModel);
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
                auto indexes = statTypeModel.match(
                    statTypeModel.index(0, 0), Qt::UserRole + 1, statType);
                if (indexes.isEmpty()) continue;
                QStandardItem *typeItem = new QStandardItem();
                typeItem->setData(statType);
                typeItem->setText(statType);
                typeItem->setEditable(false);
                typeItem->setToolTip(deletiveToolTip);
                QStandardItem *statItem = new QStandardItem();
                statItem->setText(statObj[QStringLiteral("stat")].toString());
                statItem->setToolTip(deletiveToolTip);
                QStandardItem *valueItem = new QStandardItem();
                valueItem->setData(statObj.value(QStringLiteral("value")),
                                   Qt::DisplayRole);
                valueItem->setToolTip(deletiveToolTip);

                playerStatModel.appendRow({ typeItem, statItem, valueItem });
            }
        }
    }
}

void EntityConditionDialog::onAddedEntityEffect() {
    if ((ui->effectAmpInput->isCurrentlyUnset())
        | (ui->effectDuraInput->isCurrentlyUnset()))
        return;

    QString effectText = ui->effectCombo->currentText();
    if (!entityEffectModel.findItems(effectText).isEmpty())
        return;

    QStandardItem *effectItem = new QStandardItem(effectText);
    effectItem->setData(ui->effectCombo->currentData(Qt::UserRole + 1));
    effectItem->setEditable(false);
    effectItem->setToolTip(deletiveToolTip);
    QStandardItem *amplifierItem = new QStandardItem();
    amplifierItem->setData(ui->effectAmpInput->toJson(), Qt::DisplayRole);
    amplifierItem->setToolTip(deletiveToolTip);
    QStandardItem *durationItem = new QStandardItem();
    durationItem->setData(ui->effectDuraInput->toJson(), Qt::DisplayRole);
    durationItem->setToolTip(deletiveToolTip);
    QStandardItem *ambientItem = new QStandardItem();
    ambientItem->setData(ui->effectAmbientCheck->isChecked(), Qt::DisplayRole);
    ambientItem->setToolTip(deletiveToolTip);
    QStandardItem *visibleItem = new QStandardItem();
    visibleItem->setData(ui->effectVisibleCheck->isChecked(), Qt::DisplayRole);
    visibleItem->setToolTip(deletiveToolTip);

    entityEffectModel.appendRow({ effectItem, amplifierItem, durationItem,
                                  ambientItem, visibleItem });
}

void EntityConditionDialog::onAddedPlayerAdv() {
    QString advText = ui->advanmEdit->text();

    if (!playerAdvanmModel.findItems(advText).isEmpty())
        return;

    QStandardItem *advItem = new QStandardItem(advText);
    advItem->setToolTip(deletiveToolTip);
    QStandardItem *boolItem = new QStandardItem();
    boolItem->setData(ui->advanmCheck->isChecked(), Qt::DisplayRole);
    boolItem->setToolTip(deletiveToolTip);

    playerAdvanmModel.appendRow({ advItem, boolItem });
}

void EntityConditionDialog::onAddedPlayerRecipe() {
    QString recipeText = ui->recipeEdit->text();

    if (!playerRecipeModel.findItems(recipeText).isEmpty())
        return;

    QStandardItem *recipeItem = new QStandardItem(recipeText);
    recipeItem->setToolTip(deletiveToolTip);
    QStandardItem *boolItem = new QStandardItem();
    boolItem->setData(ui->recipeCheck->isChecked(), Qt::DisplayRole);
    boolItem->setToolTip(deletiveToolTip);

    playerRecipeModel.appendRow({ recipeItem, boolItem });
}

void EntityConditionDialog::onAddedPlayerStat() {
    if (ui->statEdit->text().isEmpty()
        | (ui->statValueInput->isCurrentlyUnset()))
        return;

    QStandardItem *typeItem =
        new QStandardItem(ui->statTypeCombo->currentText());
    typeItem->setData(ui->statTypeCombo->currentData(Qt::UserRole + 1));
    typeItem->setEditable(false);
    typeItem->setToolTip(deletiveToolTip);
    QStandardItem *statItem = new QStandardItem();
    statItem->setText(ui->statEdit->text());
    statItem->setToolTip(deletiveToolTip);
    QStandardItem *valueItem = new QStandardItem();
    valueItem->setData(ui->statValueInput->toJson(), Qt::DisplayRole);
    valueItem->setToolTip(deletiveToolTip);

    playerStatModel.appendRow({ typeItem, statItem, valueItem });
}

void EntityConditionDialog::onEntityTypeChanged() {
    /*qDebug() << "onEntityTypeChanged" << ui->player; */
    if ((ui->entityTypeCombo->currentIndex() == 0)
        || (ui->entityTypeCombo->currentData(Qt::UserRole + 1).toString() ==
            QStringLiteral("minecraft:player"))) {
        if (ui->toolBox->indexOf(ui->player) == -1) {
            ui->toolBox->addItem(ui->player, tr("Player"));
        }
    } else {
        if (ui->player->parent() != nullptr) {
            QScrollArea *sv = qobject_cast<QScrollArea*>(
                ui->player->parent()->parent());
            if (sv != nullptr) {
                ui->toolBox->removeItem(ui->toolBox->indexOf(ui->player));
                ui->player->setParent(nullptr);
                delete sv;
            }
        }
    }
}

void EntityConditionDialog::initEffectsPage() {
    QStandardItem *effectItem    = new QStandardItem(tr("Effect"));
    QStandardItem *amplifierItem = new QStandardItem(tr("Amplifier"));
    QStandardItem *durationItem  = new QStandardItem(tr("Duration"));
    QStandardItem *ambientItem   = new QStandardItem(tr("Ambient"));
    QStandardItem *visibleItem   = new QStandardItem(tr("Visible"));

    ExtendedDelegate *delegate = new ExtendedDelegate(this);

    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);
    delegate->setExNumInputGeneralMin(1);

    initModelView(entityEffectModel, ui->effectTableView,
                  { effectItem, amplifierItem, durationItem, ambientItem,
                    visibleItem }, delegate);
    ui->effectTableView->setColumnWidth(0, 150);
    ui->effectTableView->setColumnWidth(1, 150);
    ui->effectTableView->setColumnWidth(2, 150);

    connect(ui->effectAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedEntityEffect);
}

void EntityConditionDialog::initPlayerAdv() {
    QStandardItem *advItem     = new QStandardItem(tr("Advancement"));
    QStandardItem *grantedItem = new QStandardItem(tr("Is granted"));

    initModelView(playerAdvanmModel, ui->advanmTableView,
                  { advItem, grantedItem });

    connect(ui->advanmAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerAdv);
}

void EntityConditionDialog::initPlayerRecipe() {
    QStandardItem *recipeItem  = new QStandardItem(tr("Recipe"));
    QStandardItem *grantedItem = new QStandardItem(tr("Is granted"));

    initModelView(playerRecipeModel, ui->recipeTableView,
                  { recipeItem, grantedItem });

    connect(ui->recipeAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerRecipe);
}

void EntityConditionDialog::initPlayerStat() {
    QStandardItem *typeItem  = new QStandardItem(tr("Type"));
    QStandardItem *statItem  = new QStandardItem(tr("Stat"));
    QStandardItem *valueItem = new QStandardItem(tr("Value"));

    ExtendedDelegate *delegate = new ExtendedDelegate(this);

    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);

    initModelView(playerStatModel, ui->statsTableView,
                  { typeItem, statItem, valueItem }, delegate);

    connect(ui->statAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerStat);
}

void EntityConditionDialog::setupGrantedTableFromJson(const QJsonObject &json,
                                                      QStandardItemModel &model)
{
    for (auto name : json.keys()) {
        QStandardItem *nameItem = new QStandardItem(name);
        nameItem->setToolTip(deletiveToolTip);
        QStandardItem *boolItem = new QStandardItem();
        boolItem->setData(json[name].toBool());
        boolItem->setToolTip(deletiveToolTip);

        model.appendRow({ nameItem, boolItem });
    }
}
