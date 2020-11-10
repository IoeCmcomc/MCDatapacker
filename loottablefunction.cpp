#include "loottablefunction.h"
#include "loottableentry.h"
#include "mcrpredcondition.h"
#include "ui_loottablefunction.h"
#include "mcrinvitem.h"
#include "mainwindow.h"

#include "extendeddelegate.h"

#include <QJsonArray>
#include <QJsonObject>

LootTableFunction::LootTableFunction(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTableFunction) {
    ui->setupUi(this);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &QObject::deleteLater);
    connect(ui->functionTypeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LootTableFunction::onTypeChanged);
    connect(ui->addCondButton, &QPushButton::clicked,
            this, &LootTableFunction::onAddCondition);

    initComboModelView("enchantment", enchantmentsModel,
                       ui->bonus_enchantCombo, false);

    ui->copyNBT_table->installEventFilter(&viewFilter);
    connect(ui->copyNBT_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::copyNBT_onAdded);

    initBlocksModel();
    ui->conditionsArea->setLayout(&conditionsLayout);

    ui->copyState_blockCombo->setModel(&blocksModel);
    ui->copyState_list->installEventFilter(&viewFilter);
    connect(ui->copyState_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::copyState_onAdded);

    ui->enchantRand_enchantCombo->setModel(&enchantmentsModel);
    ui->enchantRand_list->installEventFilter(&viewFilter);
    connect(ui->enchantRand_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::enchantRand_onAdded);

    initComboModelView("feature", featuresModel, ui->map_destCombo);
    initComboModelView("map_icon", mapIconsModel, ui->map_decoCombo);

    ui->limitCount_limitInput->setTypes(NumericInput::ExactAndRange);

    ui->lootEnchant_countInput->setTypes(NumericInput::ExactAndRange);

    ui->setAttr_ammountInput->setTypes(NumericInput::Range);
    initComboModelView("attribute", attributesModel,
                       ui->setAttr_attrCombo, false);

    ExtendedDelegate *delegate = new ExtendedDelegate(this);
    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);

    ui->setAttr_table->setItemDelegate(delegate);
    ui->setAttr_table->installEventFilter(&viewFilter);

    connect(ui->setAttr_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::setAttr_onAdded);

    ui->entriesArea->setLayout(&entriesLayout);
    connect(ui->addEntryButton, &QPushButton::clicked,
            this, &LootTableFunction::entries_onAdded);

    ui->setName_textEdit->setOneLine(true);
    ui->setName_textEdit->setDarkMode(true);

    initComboModelView("effect", effectsModel, ui->stewEffect_effectCombo);
    connect(ui->stewEffect_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::effectStew_onAdded);
}

LootTableFunction::~LootTableFunction() {
    delete ui;
}

QJsonObject LootTableFunction::toJson() const {
    QJsonObject root;

    root.insert("function", "minecraft:" +
                functTypes[ui->functionTypeCombo->currentIndex()]);

    int childCount = ui->conditionsArea->children().count();
    if (childCount != 0) {
        QJsonArray conditions;
        for (auto *child : ui->entriesContainer->children()) {
            MCRPredCondition *childCond = qobject_cast<MCRPredCondition*>(
                child);
            if (childCond != nullptr)
                conditions.push_back(childCond->toJson());
        }
        root.insert("conditions", conditions);
    }

    switch (ui->functionTypeCombo->currentIndex()) {
    case 0: { /*Apply bonus */
        root.insert("enchantment", ui->bonus_enchantCombo->currentData(
                        Qt::UserRole + 1).toString());
        int formulaIndex = ui->bonus_formulaCombo->currentIndex();
        root.insert("formula", formulaTypes[formulaIndex]);
        if (formulaIndex == 0) {
            root.insert("extra", ui->bonus_extraSpin->value());
            root.insert("probability", ui->bonus_probSpin->value());
        } else if (formulaIndex == 1) {
            root.insert("bonusMultiplier", ui->bonus_multiplierSpin->value());
        }
        break;
    }

    case 1: { /*Copy NBT */
        root.insert("source",
                    entityTargets[ui->copyNBT_entityCombo->currentIndex()]);
        QJsonArray ops;
        for (int i = 0; i < ui->copyNBT_table->rowCount(); ++i) {
            auto source = ui->copyNBT_table->item(i, 0)->text();
            auto op     = ui->copyNBT_table->item(i, 1)->
                          data(Qt::UserRole + 1).toString();
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

    case 2: { /* Copy states */
        root.insert("block", ui->copyState_blockCombo->currentData(
                        Qt::UserRole + 1).toString());
        QJsonArray properties;
        for (int i = 0; i < ui->copyState_list->count(); i++) {
            properties.append(ui->copyState_list->item(i)->text());
        }
        if (!properties.isEmpty())
            root.insert("properties", properties);
        break;
    }

    case 3: { /* Enchant randomly */
        QJsonArray enchantments;
        for (int i = 0; i < ui->enchantRand_list->count(); i++) {
            enchantments.append(ui->enchantRand_list->item(i)->data(Qt::UserRole
                                                                    + 1).toString());
        }
        if (!enchantments.isEmpty())
            root.insert("enchantments", enchantments);
        break;
    }

    case 4: { /* Enchant with level */
        ui->levelEnchant_treasureCheck->insertToJsonObject(root, "treasure");
        root.insert("levels", ui->levelEnchant_levelInput->toJson());
        break;
    }

    case 5: { /* Exploration map */
        root.insert("destination", ui->map_destCombo->currentData(Qt::UserRole +
                                                                  1).toString());
        root.insert("decoration", ui->map_decoCombo->currentData(Qt::UserRole +
                                                                 1).toString());
        root.insert("root", ui->map_zoomSpin->value());
        root.insert("search_radius", ui->map_radiusSpin->value());
        ui->map_skipExistingCheck->insertToJsonObject(root,
                                                      "skip_existing_chunks");
        break;
    }

    case 6: {   /* Fills player head */
        root.insert("entity",
                    entityTargets[ui->fillHead_entityType->currentIndex()]);
        break;
    }

    case 7: {   /* Limit count */
        root.insert("limit", ui->limitCount_limitInput->toJson());
        break;
    }

    case 8: {   /* Looting enchant */
        root.insert("count", ui->lootEnchant_countInput->toJson());
        root.insert("limit", ui->lootEnchant_limitSpin->value());
        break;
    }

    case 9: {   /* Set attributes */
        QJsonArray modifiers;
        for (int i = 0; i < ui->setAttr_table->rowCount(); i++) {
            auto name = ui->setAttr_table->item(i, 0)->text();
            auto attr = ui->setAttr_table->item(i, 1)->text();
            auto op   = ui->setAttr_table->item(i, 2)
                        ->data(Qt::UserRole + 1).toString();
            auto ammount = ui->setAttr_table->item(i, 3)
                           ->data(Qt::DisplayRole).toJsonValue();
            auto id       = ui->setAttr_table->item(i, 5)->text();
            auto slotList = QJsonArray::fromStringList(
                ui->setAttr_table->item(i, 4)
                ->data(Qt::UserRole + 1).toStringList());
            QJsonObject modifier {
                { "name", name },
                { "attribute", attr },
                { "operation", op },
                { "amount", ammount },
                { "slot", slotList },
                { "id", id }
            };
            modifiers.append(modifier);
        }
        if (!modifiers.isEmpty())
            root.insert("modifiers", modifiers);
        break;
    }

    case 10: {   /* Set contents */
        childCount = ui->entriesArea->children().count();
        if (childCount != 0) {
            QJsonArray entries;
            for (auto *child : ui->entriesContainer->children()) {
                LootTableEntry *childEntry = qobject_cast<LootTableEntry*>(
                    child);
                if (childEntry != nullptr)
                    entries.push_back(childEntry->toJson());
            }
            root.insert("entries", entries);
        }
        break;
    }

    case 11: {   /* Set count */
        root.insert("count", ui->setCount_countInput->toJson());
        break;
    }

    case 12: {   /* Set damage */
        root.insert("damage", ui->setDamage_damageInput->toJson());
        break;
    }

    case 13: {   /* Set loot table */
        if (ui->lootTable_idEdit->text().isEmpty())
            break;
        root.insert("name", ui->lootTable_idEdit->text());
        if (ui->lootTable_seedSpin->value() != 0)
            root.insert("seed", ui->lootTable_seedSpin->value());
        break;
    }

    case 14: {   /* Set lore */
        root.insert("entity",
                    entityTargets[ui->setLore_entityCombo->currentIndex()]);
        ui->setLore_replaceCheck->insertToJsonObject(root, "replace");
        QString loreText = ui->setLore_textEdit->getTextEdit()->toPlainText();
        auto    lore     = QJsonArray::fromStringList(loreText.split('\n'));
        root.insert("lore", lore);
        break;
    }

    case 15: {   /* Set name */
        root.insert("entity",
                    entityTargets[ui->setName_entityCombo->currentIndex()]);
        root.insert("name", ui->setName_textEdit->getTextEdit()->toPlainText());
        break;
    }

    case 16: {   /* Set NBT string */
        root.insert("nbt", ui->setNBT_NBTEdit->text());
        break;
    }

    case 17: {   /* Set stew effects */
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

    case 18: {   /* Copy name */
        root.insert("source", "block_entity");
    }
    }

    return root;
}

void LootTableFunction::onTypeChanged(int index) {
    const int maxIndex = ui->stackedWidget->count() - 1;

    if (index > maxIndex)
        ui->stackedWidget->setCurrentIndex(maxIndex);
    else
        ui->stackedWidget->setCurrentIndex(index);
}

void LootTableFunction::onAddCondition() {
    MCRPredCondition *cond = new MCRPredCondition(ui->conditionsContainer);

    conditionsLayout.addWidget(cond, 0);
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

    if (!ui->enchantRand_list->findItems(enchantmentText, 0).isEmpty())
        return;

    QListWidgetItem *enchantItem = new QListWidgetItem(enchantmentText);

    enchantItem->setData(Qt::UserRole + 1, ui->enchantRand_enchantCombo->
                         currentData(Qt::UserRole + 1));
    ui->enchantRand_list->addItem(enchantItem);
}

void LootTableFunction::setAttr_onAdded() {
    if (ui->setAttr_nameEdit->text().isEmpty()
        || ui->setAttr_ammountInput->isCurrentlyUnset())
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

    QTableWidgetItem *amountItem = new QTableWidgetItem();
    amountItem->setData(Qt::DisplayRole,
                        ui->setAttr_ammountInput->toJson());
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

void LootTableFunction::entries_onAdded() {
    LootTableEntry *entry = new LootTableEntry(ui->entriesContainer);

    entriesLayout.addWidget(entry, 0);
}

void LootTableFunction::effectStew_onAdded() {
    if (ui->stewEffect_effectCombo->currentIndex() == 0
        || ui->stewEffect_durationSpin->value() <= 0)
        return;

    QTableWidgetItem *effectItem = new QTableWidgetItem(
        ui->stewEffect_effectCombo->currentText());
    effectItem->setFlags(effectItem->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *durationItem = new QTableWidgetItem();
    durationItem->setData(Qt::DisplayRole,
                          ui->stewEffect_durationSpin->value());

    appendRowToTableWidget(ui->stewEffect_table,
                           { effectItem, durationItem });
}

void LootTableFunction::initBlocksModel() {
    auto blocksInfo = MainWindow::getMCRInfo("block");

    blocksModel.appendRow(new QStandardItem(tr("(not set)")));
    for (auto key : blocksInfo.keys()) {
        MCRInvItem     invItem(key);
        QStandardItem *item = new QStandardItem();
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
