#include "entityconditiondialog.h"
#include "ui_entityconditiondialog.h"

#include "extendednumericdelegate.h"

#include <QDebug>
#include <QScrollArea>

EntityConditionDialog::EntityConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::EntityConditionDialog) {
    ui->setupUi(this);
    auto typeFlag = ExtendedNumericInput::Exact
                    | ExtendedNumericInput::Range;
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

    initComboModelView("entity", entityModel, ui->entityTypeCombo);
    initComboModelView("effect", effectModel, ui->effectCombo, false);
    initComboModelView("stat_type", statTypeModel, ui->statTypeCombo, false);

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
    connect(ui->absDistanceCheck, &QCheckBox::toggled,
            ui->absDistanceInput, &ExtendedNumericInput::setEnabled);
    connect(ui->horizDistanceCheck, &QCheckBox::toggled,
            ui->horizDistanceInput, &ExtendedNumericInput::setEnabled);
    connect(ui->xDistanceCheck, &QCheckBox::toggled,
            ui->xDistanceInput, &ExtendedNumericInput::setEnabled);
    connect(ui->yDistanceCheck, &QCheckBox::toggled,
            ui->yDistanceInput, &ExtendedNumericInput::setEnabled);
    connect(ui->zDistanceCheck, &QCheckBox::toggled,
            ui->zDistanceInput, &ExtendedNumericInput::setEnabled);
}

EntityConditionDialog::~EntityConditionDialog() {
    delete ui;
}

QJsonObject EntityConditionDialog::toJson() const {
    QJsonObject root;

    if (ui->entityTypeCombo->currentIndex() != 0)
        root.insert("type", ui->entityTypeCombo->currentData(
                        Qt::UserRole + 1).toJsonValue());
    if (!ui->teamEdit->text().isEmpty())
        root.insert("team", ui->teamEdit->text());
    if (!ui->NBTEdit->text().isEmpty())
        root.insert("nbt", ui->NBTEdit->text());
    QJsonObject distance;
    if (ui->absDistanceCheck->isChecked())
        distance.insert("absolute", ui->absDistanceInput->toJson());
    if (ui->horizDistanceCheck->isChecked())
        distance.insert("horizontal", ui->horizDistanceInput->toJson());
    if (ui->xDistanceCheck->isChecked())
        distance.insert("x", ui->xDistanceInput->toJson());
    if (ui->yDistanceCheck->isChecked())
        distance.insert("y", ui->yDistanceInput->toJson());
    if (ui->zDistanceCheck->isChecked())
        distance.insert("z", ui->zDistanceInput->toJson());
    if (!distance.isEmpty())
        root.insert("distance", distance);
    QJsonObject flags;
    ui->onFireCheck->insertToJsonObject(flags, "is_on_fire");
    ui->isSprintingCheck->insertToJsonObject(flags, "is_sprinting");
    ui->isSneakingCheck->insertToJsonObject(flags, "is_sneaking");
    ui->isSwimmingCheck->insertToJsonObject(flags, "is_swimming");
    ui->isBabyCheck->insertToJsonObject(flags, "is_baby");
    if (!flags.isEmpty())
        root.insert("flags", flags);
    QJsonObject equipment;
    if (!ui->mainhandPropBtn->getData().isEmpty())
        equipment.insert("mainhand", ui->mainhandPropBtn->getData());
    if (!ui->offhandPropBtn->getData().isEmpty())
        equipment.insert("offhand", ui->offhandPropBtn->getData());
    if (!ui->headPropBtn->getData().isEmpty())
        equipment.insert("head", ui->headPropBtn->getData());
    if (!ui->chestPropBtn->getData().isEmpty())
        equipment.insert("chest", ui->chestPropBtn->getData());
    if (!ui->legsPropBtn->getData().isEmpty())
        equipment.insert("legs", ui->legsPropBtn->getData());
    if (!ui->feetPropBtn->getData().isEmpty())
        equipment.insert("feet", ui->feetPropBtn->getData()); ;
    if (!equipment.isEmpty())
        root.insert("equipment", equipment);
    if (!ui->locatPropBtn->getData().isEmpty())
        root.insert("lovation", ui->locatPropBtn->getData());

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
                       QJsonObject({ { "amplifier", amplifier },
                                       { "duration", duration },
                                       { "ambient", ambient },
                                       { "visible", visible } }));
    }
    if (!effects.isEmpty())
        root.insert("effects", effects);

    if ((ui->entityTypeCombo->currentIndex() == 0)
        || (ui->entityTypeCombo->currentData(Qt::UserRole + 1).toString() ==
            QStringLiteral("minecraft:player"))) {
        QJsonObject player;
        if (ui->gameModeCombo->currentIndex() != 0)
            player.insert("gamemode",
                          ui->gameModeCombo->currentText().toLower());
        player.insert("level", ui->playerLevelInput->toJson());

        QJsonObject advancements;
        for (int i = 0; i < playerAdvanmModel.rowCount(); ++i) {
            auto advanm =
                playerAdvanmModel.item(i, 0)->data(Qt::DisplayRole).toString();
            auto granted =
                playerAdvanmModel.item(i, 1)->data(Qt::DisplayRole).toBool();
            advancements.insert(advanm, granted);
        }
        if (!advancements.isEmpty())
            player.insert("advancements", advancements);

        QJsonObject recipes;
        for (int i = 0; i < playerRecipeModel.rowCount(); ++i) {
            auto recipe =
                playerRecipeModel.item(i, 0)->data(Qt::DisplayRole).toString();
            auto granted =
                playerRecipeModel.item(i, 1)->data(Qt::DisplayRole).toBool();
            recipes.insert(recipe, granted);
        }
        if (!recipes.isEmpty())
            player.insert("recipes", recipes);

        QJsonArray stats;
        for (int i = 0; i < playerStatModel.rowCount(); ++i) {
            auto type =
                playerStatModel.item(i, 0)->data(Qt::UserRole + 1).toString();
            auto stat =
                playerStatModel.item(i, 1)->data(Qt::DisplayRole).toString();
            auto value =
                playerStatModel.item(i, 2)->data(Qt::DisplayRole).toJsonValue();
            stats.push_back(QJsonObject({ { "type", type },
                                            { "stat", stat },
                                            { "value", value } }));
        }
        if (!stats.isEmpty())
            player.insert("stats", stats);
        if (!player.isEmpty())
            root.insert("player", player);
    }
    ;

    return root;
}

void EntityConditionDialog::fromJson(const QJsonObject &value) {
    if (value.contains("type"))
        setupComboFrom(ui->entityTypeCombo, value["type"]);
    if (value.contains("team"))
        ui->teamEdit->setText(value["team"].toString());
    if (value.contains("nbt"))
        ui->NBTEdit->setText(value["nbt"].toString());
    if (value.contains("distance")) {
        auto distance = value["distance"].toObject();
        ui->absDistanceCheck->setChecked(distance.contains("absolute"));
        if (distance.contains("absolute"))
            ui->absDistanceInput->fromJson(distance["absolute"]);
        ui->horizDistanceCheck->setChecked(distance.contains("horizontal"));
        if (distance.contains("horizontal"))
            ui->horizDistanceInput->fromJson(distance["horizontal"]);
        ui->xDistanceCheck->setChecked(distance.contains("x"));
        if (distance.contains("x"))
            ui->xDistanceInput->fromJson(distance["x"]);
        ui->yDistanceCheck->setChecked(distance.contains("y"));
        if (distance.contains("y"))
            ui->yDistanceInput->fromJson(distance["y"]);
        ui->zDistanceCheck->setChecked(distance.contains("z"));
        if (distance.contains("z"))
            ui->zDistanceInput->fromJson(distance["z"]);
    }
    if (value.contains("flags")) {
        auto flags = value["flags"].toObject();
        ui->onFireCheck->setupFromJsonObject(flags, "is_on_fire");
        ui->isSprintingCheck->setupFromJsonObject(flags, "is_sprinting");
        ui->isSneakingCheck->setupFromJsonObject(flags, "is_sneaking");
        ui->isSwimmingCheck->setupFromJsonObject(flags, "is_swimming");
        ui->isBabyCheck->setupFromJsonObject(flags, "is_baby");
    }
    if (value.contains("equipment")) {
        auto equipment = value["equipment"].toObject();
        if (equipment.contains("mainhand"))
            ui->mainhandPropBtn->setData(equipment["mainhand"].toObject());
        if (equipment.contains("offhand"))
            ui->offhandPropBtn->setData(equipment["offhand"].toObject());
        if (equipment.contains("head"))
            ui->headPropBtn->setData(equipment["head"].toObject());
        if (equipment.contains("chest"))
            ui->chestPropBtn->setData(equipment["chest"].toObject());
        if (equipment.contains("legs"))
            ui->legsPropBtn->setData(equipment["legs"].toObject());
        if (equipment.contains("feet"))
            ui->feetPropBtn->setData(equipment["feet"].toObject());
    }
    if (value.contains("location"))
        ui->locatPropBtn->setData(value["location"].toObject());
    if (value.contains("effects")) {
        auto effects = value["effects"].toObject();
        for (auto effectID : effects.keys()) {
            if (!effectID.contains(":"))
                effectID = "minecraft:" + effectID;
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
    if (value.contains("player")) {
        auto player = value["player"].toObject();
        if (player.contains("gamemode")) {
            auto gamemode = player["gamemode"].toString();
            gamemode[0].toUpper();
            setupComboFrom(ui->gameModeCombo, gamemode);
        }
        if (player.contains("level"))
            ui->playerLevelInput->fromJson(player["level"]);
        if (player.contains("advancements"))
            setupGrantedTableFromJson(player["advancements"].toObject(),
                                      playerAdvanmModel);
        if (player.contains("recipes"))
            setupGrantedTableFromJson(player["recipes"].toObject(),
                                      playerRecipeModel);
        if (player.contains("stats")) {
            auto stats = player["stats"].toArray();
            for (auto stat : stats) {
                QJsonObject statObj = stat.toObject();
                if (statObj.isEmpty())
                    continue;
                QString statType = statObj["type"].toString();
                if (!statType.contains(":"))
                    statType = "minecraft:" + statType;
                auto indexes = statTypeModel.match(
                    statTypeModel.index(0, 0), Qt::UserRole + 1, statType);
                if (indexes.isEmpty()) continue;
                QStandardItem *typeItem = new QStandardItem();
                typeItem->setData(statType);
                typeItem->setText(statType);
                typeItem->setEditable(false);
                typeItem->setToolTip(deletiveToolTip);
                QStandardItem *statItem = new QStandardItem();
                statItem->setText(statObj["stat"].toString());
                statItem->setToolTip(deletiveToolTip);
                QStandardItem *valueItem = new QStandardItem();
                valueItem->setData(statObj.value("value"), Qt::DisplayRole);
                valueItem->setToolTip(deletiveToolTip);

                playerStatModel.appendRow({ typeItem, statItem, valueItem });
            }
        }
    }
}

void EntityConditionDialog::onAddedEntityEffect() {
    if ((ui->effectAmpInput->getExactly() == 0)
        | (ui->effectDuraInput->getExactly() == 0))
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
        | (ui->statValueInput->getExactly() == 0))
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
    QStandardItem *effectItem    = new QStandardItem("Effect");
    QStandardItem *amplifierItem = new QStandardItem("Amplifier");
    QStandardItem *durationItem  = new QStandardItem("Duration");
    QStandardItem *ambientItem   = new QStandardItem("Ambient");
    QStandardItem *visibleItem   = new QStandardItem("Is visible");

    ExtendedNumericDelegate *delegate = new ExtendedNumericDelegate(this);

    delegate->setExNumInputTypes(ExtendedNumericInput::Exact
                                 | ExtendedNumericInput::Range);
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
    QStandardItem *advItem     = new QStandardItem("Advancement");
    QStandardItem *grantedItem = new QStandardItem("Is granted");

    initModelView(playerAdvanmModel, ui->advanmTableView,
                  { advItem, grantedItem });

    connect(ui->advanmAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerAdv);
}

void EntityConditionDialog::initPlayerRecipe() {
    QStandardItem *recipeItem  = new QStandardItem("Recipe");
    QStandardItem *grantedItem = new QStandardItem("Is granted");

    initModelView(playerRecipeModel, ui->recipeTableView,
                  { recipeItem, grantedItem });

    connect(ui->recipeAddBtn, &QPushButton::clicked,
            this, &EntityConditionDialog::onAddedPlayerRecipe);
}

void EntityConditionDialog::initPlayerStat() {
    QStandardItem *typeItem  = new QStandardItem("Type");
    QStandardItem *statItem  = new QStandardItem("Stat");
    QStandardItem *valueItem = new QStandardItem("Value");

    ExtendedNumericDelegate *delegate = new ExtendedNumericDelegate(this);

    delegate->setExNumInputTypes(ExtendedNumericInput::Exact
                                 | ExtendedNumericInput::Range);

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
