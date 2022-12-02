#include "loottablefunction.h"

#include "loottablecondition.h"
#include "ui_loottablefunction.h"
#include "inventoryitem.h"
#include "numberproviderdelegate.h"
#include "globalhelpers.h"
#include "modelfunctions.h"
#include "game.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringListModel>

LootTableFunction::LootTableFunction(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTableFunction) {
    ui->setupUi(this);
    updateConditionsTab(0);

    auto *view  = qobject_cast<QListView*>(ui->functionTypeCombo->view());
    auto *model =
        static_cast<QStandardItemModel*>(ui->functionTypeCombo->model());
    if (Game::version() < Game::v1_17) {
        view->setRowHidden(SetEnchantments, true);
        model->item(SetEnchantments, 0)->setEnabled(false);
        view->setRowHidden(SetBannerPattern, true);
        model->item(SetBannerPattern, 0)->setEnabled(false);

        ui->setCount_addCheck->hide();
        ui->setDamage_addCheck->hide();
    }
    if (Game::version() < Game::v1_18) {
        view->setRowHidden(SetPotion, true);
        model->item(SetPotion, 0)->setEnabled(false);

        ui->setContent_typeLabel->hide();
        ui->setContents_typeCombo->hide();
        ui->lootTable_typeLabel->hide();
        ui->lootTable_typeCombo->hide();
    } else {
        initComboModelViewFromRegistry(QStringLiteral("block_entity_type"),
                                       blockEntityTypesModel,
                                       ui->setContents_typeCombo, false);
        ui->lootTable_typeCombo->setModel(&blockEntityTypesModel);
    }

    connect(ui->functionTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &LootTableFunction::onTypeChanged);
    connect(this, &QTabWidget::currentChanged, this,
            &LootTableFunction::onTabChanged);
    connect(ui->conditionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableFunction::updateConditionsTab);

    initComboModelView("enchantment", enchantmentsModel,
                       ui->bonus_enchantCombo, false);

    ui->copyNBT_table->installEventFilter(&viewFilter);
    connect(ui->copyNBT_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::copyNBT_onAdded);

    initBlocksModel();

    ui->copyState_blockCombo->setModel(&blocksModel);
    ui->copyState_list->installEventFilter(&viewFilter);
    connect(ui->copyState_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::copyState_onAdded);

    if (Game::version() < Game::v1_17) {
        qobject_cast<QListView*>(ui->copyNBT_entityCombo->view())->setRowHidden(
            4, true);
        qobject_cast<QStandardItemModel*>(ui->copyNBT_entityCombo->model())->
        item(4, 0)->setEnabled(false);
        ui->copyNBT_storageLabel->hide();
        ui->copyNBT_storageEdit->hide();
    }
    connect(ui->copyNBT_entityCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int index){
        ui->copyNBT_storageLabel->setEnabled(index == 4);
        ui->copyNBT_storageEdit->setEnabled(index == 4);
    });


    ui->enchantRand_enchantCombo->setModel(&enchantmentsModel);
    ui->enchantRand_list->installEventFilter(&viewFilter);
    connect(ui->enchantRand_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::enchantRand_onAdded);

    if (Game::version() >= Game::v1_18_2) {
        initComboModelView(QStringLiteral("tag/configured_structure_feature"),
                           featuresModel, ui->map_destCombo,
                           true, true, true);
        initComboModelViewFromRegistry(QStringLiteral(
                                           "worldgen/configured_structure_feature"),
                                       featuresModel,
                                       ui->map_destCombo,
                                       false);
    } else {
        initComboModelView(QStringLiteral("feature"), featuresModel,
                           ui->map_destCombo);
    }
    initComboModelView(QStringLiteral("map_icon"), mapIconsModel,
                       ui->map_decoCombo);


    ui->limitCount_limitInput->setModes(NumberProvider::ExactAndRange);

    ui->lootEnchant_countInput->setModes(NumberProvider::ExactAndRange);

    ui->setAttr_amountInput->setModes(NumberProvider::Range);
    initComboModelView(QStringLiteral("attribute"), attributesModel,
                       ui->setAttr_attrCombo, false);

    NumberProviderDelegate *delegate = new NumberProviderDelegate(this);
    delegate->setInputModes(NumberProvider::ExactAndRange);

    ui->setAttr_table->setItemDelegate(delegate);
    ui->setAttr_table->installEventFilter(&viewFilter);

    connect(ui->setAttr_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::setAttr_onAdded);

    initBannerPatterns();

    ui->setEnchant_combo->setModel(&enchantmentsModel);
    ui->setEnchant_table->setJsonMode(ExtendedTableWidget::JsonMode::SimpleMap);
    ui->setEnchant_table->appendColumnMapping(QString(), ui->setEnchant_combo);
    ui->setEnchant_table->appendColumnMapping(QString(),
                                              ui->setEnchant_numberProvider);

    ui->setName_textEdit->setOneLine(true);
    ui->setName_textEdit->setDarkMode(true);

    initComboModelView(QStringLiteral("effect"),
                       effectsModel,
                       ui->stewEffect_effectCombo);
    connect(ui->stewEffect_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::effectStew_onAdded);
    ui->setPotion_potionCombo->setModel(&effectsModel);
    ui->setPotion_potionCombo->addItem("empty", "empty");
}

LootTableFunction::~LootTableFunction() {
    delete ui;
}

QJsonObject LootTableFunction::toJson() const {
    QJsonObject root;

    if (ui->functionTypeCombo->currentIndex() == -1)
        return root;

    const QString &&function = QStringLiteral("minecraft:") +
                               functTypes[ui->functionTypeCombo->currentIndex()];
    root.insert("function", function);

    switch (ui->functionTypeCombo->currentIndex()) {
        case ApplyBonus: { /*Apply bonus */
            root.insert("enchantment", ui->bonus_enchantCombo->currentData(
                            Qt::UserRole + 1).toString());
            int formulaIndex = ui->bonus_formulaCombo->currentIndex();
            root.insert("formula", formulaTypes[formulaIndex]);
            if (formulaIndex == 0) {
                root.insert("extra", ui->bonus_extraSpin->value());
                root.insert("probability", ui->bonus_probSpin->value());
            } else if (formulaIndex == 1) {
                root.insert("bonusMultiplier",
                            ui->bonus_multiplierSpin->value());
            }
            break;
        }

        case CopyNbt: { /*Copy NBT */
            if (ui->copyNBT_entityCombo->currentIndex() == 4) {
                QJsonObject source;
                source.insert("type", "storage");
                source.insert("source", ui->copyNBT_storageEdit->text());
                root.insert("source", source);
            } else {
                root.insert("source",
                            entityTargets[ui->copyNBT_entityCombo->currentIndex()]);
            }
            QJsonArray ops;
            for (int i = 0; i < ui->copyNBT_table->rowCount(); ++i) {
                auto source = ui->copyNBT_table->item(i, 0)->text();
                auto op     = ui->copyNBT_table->item(i, 1)
                              ->data(Qt::UserRole + 1).toString();
                auto target = ui->copyNBT_table->item(i, 2)->text();
                ops.push_back(
                    QJsonObject({ { QStringLiteral("source"), source },
                                    { QStringLiteral("op"), op },
                                    { QStringLiteral("target"), target } }));
            }
            if (!ops.isEmpty())
                root.insert("ops", ops);
            break;
        }

        case CopyState: { /* Copy states */
            root.insert("block",
                        qvariant_cast<InventoryItem>(
                            ui->copyState_blockCombo->currentData(
                                Qt::UserRole + 1)).getNamespacedID());
            QJsonArray properties;
            for (int i = 0; i < ui->copyState_list->count(); i++) {
                properties.append(ui->copyState_list->item(i)->text());
            }
            if (!properties.isEmpty())
                root.insert("properties", properties);
            break;
        }

        case EnchantRandomly: { /* Enchant randomly */
            QJsonArray enchantments;
            for (int i = 0; i < ui->enchantRand_list->count(); i++) {
                enchantments.append(ui->enchantRand_list->item(i)->data(Qt::
                                                                        UserRole
                                                                        + 1).toString());
            }
            if (!enchantments.isEmpty())
                root.insert("enchantments", enchantments);
            break;
        }

        case EnchantWithLevels: { /* Enchant with level */
            ui->levelEnchant_treasureCheck->insertToJsonObject(root,
                                                               "treasure");
            root.insert("levels", ui->levelEnchant_levelInput->toJson());
            break;
        }

        case ExplorationMap: { /* Exploration map */
            root.insert("destination",
                        ui->map_destCombo->currentData(Qt::UserRole +
                                                       1).toString());
            root.insert("decoration",
                        ui->map_decoCombo->currentData(Qt::UserRole +
                                                       1).toString());
            root.insert("zoom", ui->map_zoomSpin->value());
            root.insert("search_radius", ui->map_radiusSpin->value());
            ui->map_skipExistingCheck->insertToJsonObject(root,
                                                          "skip_existing_chunks");
            break;
        }

        case FillPlayerHead: { /* Fills player head */
            root.insert("entity",
                        entityTargets[ui->fillHead_entityType->currentIndex()]);
            break;
        }

        case LimitCount: { /* Limit count */
            root.insert("limit", ui->limitCount_limitInput->toJson());
            break;
        }

        case LootingEnchant: { /* Looting enchant */
            root.insert("count", ui->lootEnchant_countInput->toJson());
            root.insert("limit", ui->lootEnchant_limitSpin->value());
            break;
        }

        case SetAttributes: { /* Set attributes */
            QJsonArray modifiers;
            for (int i = 0; i < ui->setAttr_table->rowCount(); i++) {
                auto &&name = ui->setAttr_table->item(i, 0)->text();
                auto &&attr = ui->setAttr_table->item(i, 1)->text();
                auto &&op   = ui->setAttr_table->item(i, 2)
                              ->data(Qt::UserRole + 1).toString();
                auto &&amount = ui->setAttr_table->item(i, 3)->data(
                    ExtendedRole::NumberProviderRole).toJsonValue();
                auto &&id       = ui->setAttr_table->item(i, 5)->text();
                auto &&slotList = QJsonArray::fromStringList(
                    ui->setAttr_table->item(i, 4)
                    ->data(Qt::UserRole + 1).toStringList());
                QJsonValue slotValue;
                if (slotList.count() == 1)
                    slotValue = slotList[0].toString();
                else
                    slotValue = slotList;

                QJsonObject modifier {
                    { "name", std::move(name) },
                    { "attribute", std::move(attr) },
                    { "operation", std::move(op) },
                    { "amount", std::move(amount) },
                    { "slot", std::move(slotValue) },
                };
                if (!id.isEmpty())
                    modifier.insert("id", std::move(id));
                modifiers.append(std::move(modifier));
            }
            if (!modifiers.isEmpty())
                root.insert("modifiers", std::move(modifiers));
            break;
        }

        case SetBannerPattern: { /* Set banner pattern */
            root["patterns"] = ui->setPattern_table->toJsonArray();
            ui->setPattern_appendCheck->insertToJsonObject(root, "append");
            break;
        }

        case SetContents: { /* Set contents */
            const auto entries = ui->entryInterface->json();
            if (!entries.isEmpty()) {
                root.insert("entries", entries);
            }
            if (Game::version() >= Game::v1_18) {
                root.insert("type", ui->setContents_typeCombo->currentText());
            }
            break;
        }

        case SetCount: { /* Set count */
            root.insert("count", ui->setCount_countInput->toJson());
            if (Game::version() >= Game::v1_18) {
                ui->setCount_addCheck->insertToJsonObject(root, "add");
            }
            break;
        }

        case SetDamage: { /* Set damage */
            root.insert("damage", ui->setDamage_damageInput->toJson());
            if (Game::version() >= Game::v1_18) {
                ui->setDamage_addCheck->insertToJsonObject(root, "add");
            }
            break;
        }

        case SetEnchantments: { /* Set enchantments */
            root["enchantments"] = ui->setEnchant_table->toJsonObject();
            ui->setEnchant_addCheck->insertToJsonObject(root, "add");
            break;
        }

        case SetLootTable: { /* Set loot table */
            if (ui->lootTable_idEdit->text().isEmpty())
                break;
            root.insert("name", ui->lootTable_idEdit->text());
            if (ui->lootTable_seedSpin->value() != 0)
                root.insert("seed", ui->lootTable_seedSpin->value());
            if (Game::version() >= Game::v1_18) {
                root.insert("type", ui->lootTable_typeCombo->currentText());
            }
            break;
        }

        case SetLore: { /* Set lore */
            root.insert("entity",
                        entityTargets[ui->setLore_entityCombo->currentIndex()]);
            ui->setLore_replaceCheck->insertToJsonObject(root, "replace");
            root.insert("lore", ui->setLore_textEdit->toJson());
            break;
        }

        case SetName: { /* Set name */
            root.insert("entity",
                        entityTargets[ui->setName_entityCombo->currentIndex()]);
            /*root.insert("name", ui->setName_textEdit->getTextEdit()->toPlainText()); */
            root.insert("name", ui->setName_textEdit->toJson());
            break;
        }

        case SetNbt: { /* Set NBT string */
            root.insert("tag", ui->setNBT_NBTEdit->text());
            break;
        }

        case SetPotion: {
            if (ui->setPotion_potionCombo->currentIndex() != 0) {
                root.insert("potion",
                            ui->setPotion_potionCombo->currentData(
                                Qt::UserRole + 1).toString());
            }
            break;
        }

        case SetStewEffect: { /* Set stew effects */
            QJsonArray effects;
            for (int i = 0; i < ui->stewEffect_table->rowCount(); ++i) {
                auto type = ui->stewEffect_table->item(i, 0)
                            ->data(Qt::UserRole + 1).toString();
                auto duration = ui->stewEffect_table->item(i, 1)
                                ->data(Qt::DisplayRole).toInt();
                effects.push_back(QJsonObject({
                    { QStringLiteral("type"), type },
                    { QStringLiteral("duration"), duration }
                }));
            }
            if (!effects.isEmpty())
                root.insert("effects", effects);
            break;
        }

        case CopyName: { /* Copy name */
            root.insert("source", "block_entity");
            break;
        }

        default: {
            break;
        }
    }

    const auto &&conditions = ui->conditionsInterface->json();
    if (!conditions.isEmpty())
        root.insert("conditions", conditions);

    return root;
}

void LootTableFunction::fromJson(const QJsonObject &root) {
    if (!root.contains(QLatin1String("function")))
        return;

    QString function = root.value(QLatin1String("function")).toString();
    Glhp::removePrefix(function, QLatin1String("minecraft:"));

    const int index = functTypes.indexOf(function);

    const auto &&model =
        static_cast<QStandardItemModel*>(ui->functionTypeCombo->model());
    const auto &&item = model->item(index, 0);
    if (!item->isEnabled())
        return;

    ui->functionTypeCombo->setCurrentIndex(index);

    switch (index) {
        case ApplyBonus: { /*Apply bonus */
            if (!(root.contains("enchantment") && root.contains("formula")))
                return;

            setupComboFrom(ui->bonus_enchantCombo, root.value("enchantment"));
            int formulaIndex =
                formulaTypes.indexOf(root.value("formula").toString());

            ui->bonus_formulaCombo->setCurrentIndex(formulaIndex);
            if (formulaIndex == 0) {
                if (root.contains("extra"))
                    ui->bonus_extraSpin->setValue(root.value("extra").toInt());
                if (root.contains("probability"))
                    ui->bonus_probSpin->setValue(root.value(
                                                     "probability").toInt());
            } else if (formulaIndex == 1) {
                if (root.contains("bonusMultiplier"))
                    ui->bonus_multiplierSpin->setValue(root.value(
                                                           "bonusMultiplier").toInt());
            }
            break;
        }

        case CopyNbt: { /*Copy NBT */
            if (!root.contains("source"))
                return;

            if (root.value("source").isObject() &&
                (Game::version() >= Game::v1_17)) {
                const auto &&source = root.value("source").toObject();
                if (!(source.contains("type") && source.contains("source")))
                    return;

                QString &&type = source.value("type").toString();
                Glhp::removePrefix(type, "minecraft:");

                if (type == "storage") {
                    ui->copyNBT_entityCombo->setCurrentIndex(4);
                    ui->copyNBT_storageEdit->setText(source.value(
                                                         "source").toString());
                }
            } else {
                ui->copyNBT_entityCombo->setCurrentIndex(
                    entityTargets.indexOf(root.value("source").toString()));
            }

            if (root.contains("ops")) {
                ui->copyNBT_table->clearContents();
                QJsonArray ops = root.value("ops").toArray();
                for (auto opRef: ops) {
                    if (!opRef.isObject())
                        continue;
                    auto op = opRef.toObject();
                    if (!(op.contains("source") && op.contains("target")))
                        continue;
                    QTableWidgetItem *srcItem = new QTableWidgetItem(
                        op.value("source").toString());
                    QString opText = op.value("op").toString();
                    if (nbtOperationTypes.indexOf(opText) == -1) {
                        delete srcItem;
                        continue;
                    }
                    QTableWidgetItem *opItem = new QTableWidgetItem(
                        ui->copyNBT_opCombo->itemText(
                            nbtOperationTypes.indexOf(opText)));
                    opItem->setData(Qt::UserRole + 1, opText);
                    opItem->setFlags(opItem->flags() & ~Qt::ItemIsEditable);
                    QTableWidgetItem *targetItem = new QTableWidgetItem(
                        op.value("target").toString());
                    appendRowToTableWidget(ui->copyNBT_table,
                                           { srcItem, opItem, targetItem });
                }
            }
            break;
        }

        case CopyState: { /* Copy states */
            if (!root.contains("block"))
                return;

            QVariant vari;
            vari.setValue(InventoryItem(root.value("block").toString()));

            setupComboFrom(ui->copyState_blockCombo, vari);

            if (root.contains("properties")) {
                QJsonArray properties = root.value("properties").toArray();
                for (auto propRef : properties) {
                    ui->copyState_list->addItem(propRef.toString());
                }
            }
            break;
        }

        case EnchantRandomly: { /* Enchant randomly */
            if (root.contains("enchantments")) {
                QJsonArray enchantments = root.value("enchantments").toArray();
                for (auto enchantmentRef : enchantments) {
                    auto enchantmentId  = enchantmentRef.toString();
                    int  enchantmentIdx = ui->enchantRand_enchantCombo->
                                          findData(enchantmentId,
                                                   Qt::UserRole + 1);
                    if (enchantmentIdx == -1)
                        continue;

                    QListWidgetItem *enchantItem = new QListWidgetItem(
                        ui->enchantRand_enchantCombo->itemText(enchantmentIdx));

                    enchantItem->setData(Qt::UserRole + 1,
                                         enchantmentId);
                    ui->enchantRand_list->addItem(enchantItem);
                }
            }
            break;
        }

        case EnchantWithLevels: { /* Enchant with level */
            if (!root.contains("levels"))
                return;

            ui->levelEnchant_treasureCheck->setupFromJsonObject(root,
                                                                "treasure");
            ui->levelEnchant_levelInput->fromJson(root.value("levels"));
            break;
        }

        case ExplorationMap: { /* Exploration map */
            setupComboFrom(ui->map_destCombo,
                           root.value("destination").toString());
            setupComboFrom(ui->map_decoCombo,
                           root.value("decoration").toString());

            if (root.contains("zoom"))
                ui->map_zoomSpin->setValue(root.value("zoom").toInt());
            if (root.contains("search_radius"))
                ui->map_radiusSpin->setValue(root.value("search_radius").toInt());

            ui->map_skipExistingCheck
            ->setupFromJsonObject(root, "skip_existing_chunks");
            break;
        }

        case FillPlayerHead: { /* Fills player head */
            if (!root.contains("entity"))
                return;

            ui->fillHead_entityType->setCurrentIndex(
                entityTargets.indexOf(root.value("entity").toString()));
            break;
        }

        case LimitCount: { /* Limit count */
            if (!root.contains("limit"))
                return;

            ui->limitCount_limitInput->fromJson(root.value("limit"));
            break;
        }

        case LootingEnchant: { /* Looting enchant */
            if (!root.contains("count"))
                return;

            ui->lootEnchant_countInput->fromJson(root.value("count"));

            if (root.contains("limit"))
                ui->lootEnchant_limitSpin->setValue(root.value("limit").toInt());
            break;
        }

        case SetAttributes: { /* Set attributes */
            if (!root.contains("modifiers"))
                return;

            ui->setAttr_table->clearContents();
            ui->setAttr_table->setRowCount(0);

            QJsonArray modifiers = root.value("modifiers").toArray();
            for (auto modifierRef: modifiers) {
                auto modifier = modifierRef.toObject();

                if (!(modifier.contains("attribute")
                      && modifier.contains("name")
                      && modifier.contains("amount")
                      && modifier.contains("operation")
                      && modifier.contains("slot"))) {
                    continue;
                }

                QTableWidgetItem *nameItem = new QTableWidgetItem(
                    modifier.value("name").toString());

                auto attrId  = modifier.value("attribute").toString();
                int  attrIdx = ui->setAttr_attrCombo->findText(attrId);
                if (attrIdx == -1) {
                    delete nameItem;
                    continue;
                }

                QTableWidgetItem *attrItem = new QTableWidgetItem(
                    ui->setAttr_attrCombo->itemText(attrIdx));
                attrItem->setFlags(attrItem->flags() & ~Qt::ItemIsEditable);

                auto opId  = modifier.value("operation").toString();
                int  opIdx = operationTypes.indexOf(opId);
                if (opIdx == -1) {
                    delete nameItem;
                    continue;
                }

                QTableWidgetItem *opItem = new QTableWidgetItem(
                    ui->setAttr_opCombo->itemText(opIdx));
                opItem->setData(Qt::UserRole + 1, opId);
                opItem->setFlags(opItem->flags() & ~Qt::ItemIsEditable);

                QTableWidgetItem *IDItem = new QTableWidgetItem(
                    (modifier.contains("id"))
                    ? modifier.value("id").toString() : "");

                auto *amountItem = new QTableWidgetItem();
                amountItem->setData(ExtendedRole::NumberProviderRole,
                                    modifier.value("amount"));
                amountItem->setFlags(amountItem->flags() & ~Qt::ItemIsEditable);

                auto        slotsVal = modifier.value("slot");
                QStringList slotsList;
                if (slotsVal.isString()) {
                    QString slot = slotsVal.toString();
                    if (slotTypes.contains(slot))
                        slotsList << slot;
                } else if (slotsVal.isArray()) {
                    for (auto slotRef: slotsVal.toArray()) {
                        QString slot = slotRef.toString();
                        if (slotTypes.contains(slot)) {
                            slotsList << slot;
                        }
                    }
                }

                QTableWidgetItem *slotsItem =
                    new QTableWidgetItem(slotsList.join(", "));
                slotsItem->setData(Qt::UserRole + 1, slotsList);
                slotsItem->setFlags(slotsItem->flags() & ~Qt::ItemIsEditable);

                appendRowToTableWidget(ui->setAttr_table,
                                       { nameItem, attrItem, opItem, amountItem,
                                         slotsItem, IDItem });
            }
            break;
        }

        case SetBannerPattern: { /* Set banner pattern */
            if (Game::version() >= Game::v1_17) {
                ui->setPattern_table->fromJson(root["patterns"].toArray());
                ui->setPattern_appendCheck->setupFromJsonObject(root, "append");
            }
            break;
        }

        case SetContents: { /* Set contents */
            if (Game::version() >= Game::v1_18) {
                if (!root.contains("type")) {
                    return;
                }
                setupComboFrom(ui->setContents_typeCombo,
                               root["type"].toString());
            }

            if (root.contains("entries")) {
                if (!ui->entryInterface->mainWidget())
                    initEntryInterface();
                ui->entryInterface->setJson(root["entries"].toArray());
            }
            break;
        }

        case SetCount: { /* Set count */
            if (!root.contains("count"))
                return;

            ui->setCount_countInput->fromJson(root.value("count"));
            if (root.contains("add")) {
                ui->setCount_addCheck->setupFromJsonObject(root, "add");
            }
            break;
        }

        case SetDamage: { /* Set damage */
            if (!root.contains("damage"))
                return;

            ui->setDamage_damageInput->fromJson(root.value(QLatin1String(
                                                               "damage")));
            if (root.contains("add") && (Game::version() >= Game::v1_17)) {
                ui->setDamage_addCheck->setupFromJsonObject(root, "add");
            }
            break;
        }

        case SetEnchantments: { /* Set enchantments */
            ui->setEnchant_table->fromJson(root["enchantments"].toObject());
            if (root.contains("add") && (Game::version() >= Game::v1_17)) {
                ui->setEnchant_addCheck->setupFromJsonObject(root, "add");
            }
            break;
        }

        case SetLootTable: { /* Set loot table */
            if (!root.contains("name"))
                return;

            if (Game::version() >= Game::v1_18) {
                if (!root.contains("type")) {
                    return;
                }
                setupComboFrom(ui->lootTable_typeCombo,
                               root["type"].toString());
            }

            ui->lootTable_idEdit->setText(root.value("name").toString());

            if (root.contains("seed"))
                ui->lootTable_seedSpin->setValue(root.value("seed").toInt());
            break;
        }

        case SetLore: { /* Set lore */
            if (root.contains("entity"))
                ui->setLore_entityCombo->setCurrentIndex(
                    entityTargets.indexOf(root.value("entity").toString()));

            ui->setLore_replaceCheck->setupFromJsonObject(root, "replace");

            if (root.contains("lore")) {
                ui->setLore_textEdit->fromJson(root.value("lore").toArray());
            }
            break;
        }

        case SetName: { /* Set name */
            if (root.contains("entity"))
                ui->setName_entityCombo->setCurrentIndex(
                    entityTargets.indexOf(root.value("entity").toString()));

            if (root.contains("name"))
/*
              ui->setName_textEdit->getTextEdit()->setPlainText(
                  root.value("name").toString());
 */
                ui->setName_textEdit->fromJson(root.value("name"));
            break;
        }

        case SetNbt: { /* Set NBT string */
            if (!root.contains("tag"))
                return;

            ui->setNBT_NBTEdit->setText(root.value("tag").toString());
            break;
        }

        case SetPotion: {
            if (!root.contains("potion"))
                return;

            if (Game::version() >= Game::v1_18) {
                setupComboFrom(ui->setPotion_potionCombo,
                               root["potion"].toString());
            }
            break;
        }

        case SetStewEffect: { /* Set stew effects */
            if (!root.contains("effects"))
                return;

            ui->stewEffect_table->clearContents();
            ui->stewEffect_table->setRowCount(0);

            QJsonArray effects = root.value("effects").toArray();
            for (auto effectRef: effects) {
                auto effect = effectRef.toObject();
                if (!effect.contains("type"))
                    continue;
                auto typeId  = effect.value("type").toString();
                int  typeIdx = ui->stewEffect_effectCombo->
                               findData(typeId, Qt::UserRole + 1);
                if (typeIdx == -1)
                    continue;

                QTableWidgetItem *effectItem = new QTableWidgetItem(
                    ui->stewEffect_effectCombo->itemText(typeIdx));
                effectItem->setData(Qt::UserRole + 1,
                                    ui->stewEffect_effectCombo->
                                    currentData(Qt::UserRole + 1));
                effectItem->setFlags(effectItem->flags() & ~Qt::ItemIsEditable);

                if (!effect.contains("duration"))
                    continue;

                auto *durationItem = new QTableWidgetItem();
                durationItem->setData(Qt::DisplayRole,
                                      effect.value("duration").toInt());

                appendRowToTableWidget(ui->stewEffect_table,
                                       { effectItem, durationItem });
            }
            break;
        }

        case CopyName: { /* Copy name */
            if (!root.contains("source"))
                return;

            auto source = root.value("source").toString();
            if (source != "block_entity")
                return;

            break;
        }
    }

    if (root.contains("conditions")) {
        if (!ui->conditionsInterface->mainWidget())
            initCondInterface();
        ui->conditionsInterface->setJson(root.value("conditions").toArray());
    }
}

void LootTableFunction::changeEvent(QEvent *event) {
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateConditionsTab(ui->conditionsInterface->entriesCount());
    }
}

void LootTableFunction::onTypeChanged(int index) {
    const int maxIndex = ui->stackedWidget->count() - 1;

    if (index > maxIndex)
        ui->stackedWidget->setCurrentIndex(maxIndex);
    else
        ui->stackedWidget->setCurrentIndex(index);
    if ((index == SetContents) && !ui->entryInterface->mainWidget())
        initEntryInterface();
}

void LootTableFunction::onTabChanged(int index) {
    if ((index == 1) && (!ui->conditionsInterface->mainWidget()))
        initCondInterface();
}

void LootTableFunction::copyNBT_onAdded() {
    if (!ui->copyNBT_sourceEdit->text().isEmpty()
        && !ui->copyNBT_targetEdit->text().isEmpty()) {
        QTableWidgetItem *srcItem = new QTableWidgetItem(
            ui->copyNBT_sourceEdit->text());
        QTableWidgetItem *opItem = new QTableWidgetItem(
            ui->copyNBT_opCombo->currentText());
        opItem->setData(Qt::UserRole + 1, ui->copyNBT_opCombo->currentIndex());
        opItem->setFlags(opItem->flags() & ~Qt::ItemIsEditable);
        QTableWidgetItem *targetItem = new QTableWidgetItem(
            ui->copyNBT_targetEdit->text());
        appendRowToTableWidget(ui->copyNBT_table,
                               { srcItem, opItem, targetItem });
    }
}

void LootTableFunction::copyState_onAdded() {
    if (!ui->copyState_lineEdit->text().isEmpty())
        ui->copyState_list->addItem(ui->copyState_lineEdit->text());
}

void LootTableFunction::enchantRand_onAdded() {
    QString enchantmentText = ui->enchantRand_enchantCombo->currentText();

    if (!ui->enchantRand_list->findItems(enchantmentText,
                                         Qt::MatchExactly).isEmpty())
        return;

    auto *enchantItem = new QListWidgetItem(enchantmentText);

    enchantItem->setData(Qt::UserRole + 1, ui->enchantRand_enchantCombo->
                         currentData(Qt::UserRole + 1));
    ui->enchantRand_list->addItem(enchantItem);
}

void LootTableFunction::setAttr_onAdded() {
    if (ui->setAttr_nameEdit->text().isEmpty()
        || ui->setAttr_amountInput->isCurrentlyUnset())
        return;

    QTableWidgetItem *nameItem = new QTableWidgetItem(
        ui->setAttr_nameEdit->text());

    QTableWidgetItem *attrItem = new QTableWidgetItem(
        ui->setAttr_attrCombo->currentText());
    attrItem->setFlags(attrItem->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *opItem = new QTableWidgetItem(
        ui->setAttr_opCombo->currentText());
    opItem->setData(Qt::UserRole + 1,
                    operationTypes[ui->setAttr_opCombo->currentIndex()]);
    opItem->setFlags(opItem->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *IDItem = new QTableWidgetItem(
        ui->setAttr_IDEdit->text());

    auto *amountItem = new QTableWidgetItem();
    amountItem->setData(ExtendedRole::NumberProviderRole,
                        ui->setAttr_amountInput->toJson());
    amountItem->setFlags(amountItem->flags() & ~Qt::ItemIsEditable);

    QStringList slotsList;
    if (ui->setAttr_mainhandCheck->isChecked())
        slotsList << "mainhand";
    if (ui->setAttr_offhandCheck->isChecked())
        slotsList << "offhand";
    if (ui->setAttr_headCheck->isChecked())
        slotsList << "head";
    if (ui->setAttr_chestCheck->isChecked())
        slotsList << "chest";
    if (ui->setAttr_legsCheck->isChecked())
        slotsList << "legs";
    if (ui->setAttr_feetCheck->isChecked())
        slotsList << "feet";

    QTableWidgetItem *slotsItem = new QTableWidgetItem(slotsList.join(", "));
    slotsItem->setData(Qt::UserRole + 1, slotsList);
    slotsItem->setFlags(slotsItem->flags() & ~Qt::ItemIsEditable);

    appendRowToTableWidget(ui->setAttr_table,
                           { nameItem, attrItem, opItem, amountItem,
                             slotsItem, IDItem });
}

void LootTableFunction::effectStew_onAdded() {
    if (ui->stewEffect_effectCombo->currentIndex() == 0
        || ui->stewEffect_durationSpin->value() <= 0)
        return;

    QTableWidgetItem *effectItem = new QTableWidgetItem(
        ui->stewEffect_effectCombo->currentText());
    effectItem->setData(Qt::UserRole + 1,
                        ui->stewEffect_effectCombo->currentData(
                            Qt::UserRole + 1));
    effectItem->setFlags(effectItem->flags() & ~Qt::ItemIsEditable);

    auto *durationItem = new QTableWidgetItem();
    durationItem->setData(Qt::DisplayRole,
                          ui->stewEffect_durationSpin->value());

    appendRowToTableWidget(ui->stewEffect_table,
                           { effectItem, durationItem });
}

void LootTableFunction::updateConditionsTab(int size) {
    setTabText(1, tr("Conditions (%1)").arg(size));
}

void LootTableFunction::initBlocksModel() {
    auto blocksInfo = Game::getInfo("block");

    blocksModel.appendRow(new QStandardItem(tr("(not set)")));
    for (const auto &key : blocksInfo.keys()) {
        InventoryItem invItem(key);
        auto         *item = new QStandardItem();
        item->setIcon(QIcon(invItem.getPixmap()));
        if (invItem.getName().isEmpty()) {
            auto name = blocksInfo.value(key).toMap().value("name").toString();
            item->setText(name);
        } else {
            item->setText(invItem.getName());
        }
        QVariant vari;
        vari.setValue(invItem);
        item->setData(vari, Qt::UserRole + 1);
        blocksModel.appendRow(item);
    }
}

void LootTableFunction::initCondInterface() {
    auto *cond = new LootTableCondition();

    ui->conditionsInterface->setMainWidget(cond);

    ui->conditionsInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&LootTableCondition::fromJson));
    ui->conditionsInterface->mapToGetter(&LootTableCondition::toJson, cond);
}

void LootTableFunction::initEntryInterface() {
    auto *entry = new LootTableFunction();

    ui->entryInterface->setupMainWidget(entry);
}

void LootTableFunction::initBannerPatterns() {
    static QStringListModel patternModel({
        "base", "square_bottom_left", "square_bottom_right", "square_top_left",
        "square_top_right", "stripe_bottom", "stripe_top", "stripe_left",
        "stripe_right", "stripe_center", "stripe_middle", "stripe_downright",
        "stripe_downleft", "small_stripes", "cross", "straight_cross",
        "triangle_bottom", "triangle_top", "triangles_bottom", "triangles_top",
        "diagonal_left", "diagonal_up_right", "diagonal_up_left",
        "diagonal_right", "circle", "rhombus", "half_vertical",
        "half_horizontal", "half_vertical_right", "half_horizontal_bottom",
        "border", "curly_border", "gradient", "gradient_up", "bricks", "globe",
        "creeper", "skull", "flower", "mojang", "piglin",
    });

    ui->setPattern_patternCombo->setModel(&patternModel);

    const auto &colors = Glhp::colorHexes;
    for (auto it = colors.cbegin(); it != colors.cend(); ++it) {
        QPixmap pixmap(16, 16);
        pixmap.fill(it.value());
        ui->setPattern_colorCombo->addItem(pixmap, it.key());
    }

    ui->setPattern_table->setJsonMode(ExtendedTableWidget::JsonMode::List);
    ui->setPattern_table->appendColumnMapping("pattern",
                                              ui->setPattern_patternCombo);
    ui->setPattern_table->appendColumnMapping("color",
                                              ui->setPattern_colorCombo);
}
