#include "loottablefunction.h"
#include "loottableentry.h"

#include "mcrpredcondition.h"
#include "ui_loottablefunction.h"
#include "mcrinvitem.h"
#include "mainwindow.h"
#include "extendeddelegate.h"
#include "globalhelpers.h"
#include "modelfunctions.h"

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

    ui->setAttr_amountInput->setTypes(NumericInput::Range);
    initComboModelView("attribute", attributesModel,
                       ui->setAttr_attrCombo, false);

    ExtendedDelegate *delegate = new ExtendedDelegate(this);
    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);

    ui->setAttr_table->setItemDelegate(delegate);
    ui->setAttr_table->installEventFilter(&viewFilter);

    connect(ui->setAttr_addBtn, &QPushButton::clicked,
            this, &LootTableFunction::setAttr_onAdded);

    ui->entriesContainer->setLayout(&entriesLayout);
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
            auto op     = entityTargets[ui->copyNBT_table->item(i, 1)->
                                        data(Qt::UserRole + 1).toInt()];
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
        root.insert("block",
                    qvariant_cast<MCRInvItem>(
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
        root.insert("zoom", ui->map_zoomSpin->value());
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
            auto amount = ui->setAttr_table->item(i, 3)
                          ->data(Qt::DisplayRole).toJsonValue();
            auto id       = ui->setAttr_table->item(i, 5)->text();
            auto slotList = QJsonArray::fromStringList(
                ui->setAttr_table->item(i, 4)
                ->data(Qt::UserRole + 1).toStringList());
            QJsonValue slotValue;
            if (slotList.count() == 1)
                slotValue = slotList[0].toString();
            else
                slotValue = slotList;

            QJsonObject modifier {
                { "name", name },
                { "attribute", attr },
                { "operation", op },
                { "amount", amount },
                { "slot", slotValue },
            };
            if (!id.isEmpty())
                modifier.insert("id", id);
            modifiers.append(modifier);
        }
        if (!modifiers.isEmpty())
            root.insert("modifiers", modifiers);
        break;
    }

    case 10: {   /* Set contents */
        auto entries = Glhp::getJsonFromObjectsFromParent<LootTableEntry>(
            ui->entriesContainer);
        if (!entries.isEmpty())
            root.insert("entries", entries);
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
/*
          QString loreText = ui->setLore_textEdit->getTextEdit()->toPlainText();
          auto    lore     = QJsonArray::fromStringList(loreText.split('\n'));
          root.insert("lore", lore);
 */
        root.insert("lore", ui->setLore_textEdit->toJson());
        break;
    }

    case 15: {   /* Set name */
        root.insert("entity",
                    entityTargets[ui->setName_entityCombo->currentIndex()]);
        /*root.insert("name", ui->setName_textEdit->getTextEdit()->toPlainText()); */
        root.insert("name", ui->setName_textEdit->toJson());
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

    auto conditions = Glhp::getJsonFromObjectsFromParent<MCRPredCondition>(
        ui->conditionsContainer);
    if (!conditions.isEmpty())
        root.insert("entries", conditions);

    return root;
}

void LootTableFunction::fromJson(const QJsonObject &root) {
    if (!root.contains("function"))
        return;

    QString function = root.value("function").toString();
    Glhp::removePrefix(function, "minecraft:");

    const int index = functTypes.indexOf(function);
/*
      if (index == -1)
          return;
 */

    ui->functionTypeCombo->setCurrentIndex(index);

    switch (index) {
    case 0: { /*Apply bonus */
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
                ui->bonus_probSpin->setValue(root.value("probability").toInt());
        } else if (formulaIndex == 1) {
            if (root.contains("bonusMultiplier"))
                ui->bonus_multiplierSpin->setValue(root.value(
                                                       "bonusMultiplier").toInt());
        }
        break;
    }

    case 1: { /*Copy NBT */
        if (!root.contains("source"))
            return;

        ui->copyNBT_entityCombo->setCurrentIndex(
            entityTargets.indexOf(root.value("source").toString()));

        if (root.contains("ops")) {
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
                if (operationTypes.indexOf(opText) == -1)
                    continue;
                QTableWidgetItem *opItem = new QTableWidgetItem(
                    ui->copyNBT_opCombo->itemText(
                        operationTypes.indexOf(opText)));
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

    case 2: { /* Copy states */
        if (!root.contains("block"))
            return;

        QVariant vari;
        vari.setValue(MCRInvItem(root.value("block").toString()));

        setupComboFrom(ui->copyState_blockCombo, vari);

        if (root.contains("properties")) {
            QJsonArray properties = root.value("properties").toArray();
            for (auto propRef : properties) {
                ui->copyState_list->addItem(propRef.toString());
            }
        }
        break;
    }

    case 3: { /* Enchant randomly */
        if (root.contains("enchantments")) {
            QJsonArray enchantments = root.value("enchantments").toArray();
            for (auto enchantmentRef : enchantments) {
                auto enchantmentId  = enchantmentRef.toString();
                int  enchantmentIdx = ui->enchantRand_enchantCombo->
                                      findData(enchantmentId, Qt::UserRole + 1);
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

    case 4: { /* Enchant with level */
        if (!root.contains("levels"))
            return;

        ui->levelEnchant_treasureCheck->setupFromJsonObject(root, "treasure");
        ui->levelEnchant_levelInput->fromJson(root.value("levels"));
        break;
    }

    case 5: { /* Exploration map */
        setupComboFrom(ui->map_destCombo, root.value("destination").toString());
        setupComboFrom(ui->map_decoCombo, root.value("decoration").toString());

        if (root.contains("zoom"))
            ui->map_zoomSpin->setValue(root.value("zoom").toInt());
        if (root.contains("search_radius"))
            ui->map_radiusSpin->setValue(root.value("search_radius").toInt());

        ui->map_skipExistingCheck
        ->setupFromJsonObject(root, "skip_existing_chunks");
        break;
    }

    case 6: {   /* Fills player head */
        if (!root.contains("entity"))
            return;

        ui->fillHead_entityType->setCurrentIndex(
            entityTargets.indexOf(root.value("entity").toString()));
        break;
    }

    case 7: {   /* Limit count */
        if (!root.contains("limit"))
            return;

        ui->limitCount_limitInput->fromJson(root.value("limit"));
        break;
    }

    case 8: {   /* Looting enchant */
        if (!root.contains("count"))
            return;

        ui->lootEnchant_countInput->fromJson(root.value("count"));

        if (root.contains("limit"))
            ui->lootEnchant_limitSpin->setValue(root.value("limit").toInt());
        break;
    }

    case 9: {   /* Set attributes */
        if (!root.contains("modifiers"))
            return;

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
            if (attrIdx == -1)
                continue;

            QTableWidgetItem *attrItem = new QTableWidgetItem(
                ui->setAttr_attrCombo->itemText(attrIdx));
            attrItem->setFlags(attrItem->flags() & ~Qt::ItemIsEditable);

            auto opId  = modifier.value("operation").toString();
            int  opIdx = operationTypes.indexOf(opId);
            if (opIdx == -1)
                continue;

            QTableWidgetItem *opItem = new QTableWidgetItem(
                ui->setAttr_opCombo->itemText(opIdx));
            opItem->setData(Qt::UserRole + 1, opId);
            opItem->setFlags(opItem->flags() & ~Qt::ItemIsEditable);

            QTableWidgetItem *IDItem = new QTableWidgetItem(
                (modifier.contains("id"))
                    ? modifier.value("id").toString() : "");

            auto *amountItem = new QTableWidgetItem();
            amountItem->setData(Qt::DisplayRole,
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
                    if (slotTypes.contains(slot))
                        slotsList << slot;
                    else
                        continue;
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

    case 10: {   /* Set contents */
        if (root.contains("entries")) {
            QJsonArray children = root.value("entries").toArray();
            Glhp::loadJsonToObjectsToLayout<LootTableEntry>(children,
                                                            entriesLayout);
        }
        break;
    }

    case 11: {   /* Set count */
        if (!root.contains("count"))
            return;

        ui->setCount_countInput->fromJson(root.value("count"));
        break;
    }

    case 12: {   /* Set damage */
        if (!root.contains("damage"))
            return;

        ui->setDamage_damageInput->fromJson(root.value("damage"));
        break;
    }

    case 13: {   /* Set loot table */
        if (!root.contains("name"))
            return;

        ui->lootTable_idEdit->setText(root.value("name").toString());

        if (root.contains("seed"))
            ui->lootTable_seedSpin->setValue(root.value("seed").toInt());
        break;
    }

    case 14: {   /* Set lore */
        if (root.contains("entity"))
            ui->setLore_entityCombo->setCurrentIndex(
                entityTargets.indexOf(root.value("entity").toString()));

        ui->setLore_replaceCheck->setupFromJsonObject(root, "replace");

        if (root.contains("lore")) {
            auto lore = QVariantList(root.value("lore")
                                     .toArray().toVariantList()).toVector();
            auto *textEdit = ui->setLore_textEdit->getTextEdit();
            textEdit->clear();
            for (int i = 0; i < lore.count(); i++)
                textEdit->append(lore[i].toString());
        }
        break;
    }

    case 15: {   /* Set name */
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

    case 16: {   /* Set NBT string */
        if (!root.contains("nbt"))
            return;

        ui->setNBT_NBTEdit->setText(root.value("nbt").toString());
        break;
    }

    case 17: {   /* Set stew effects */
        if (!root.contains("effects"))
            return;

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
            effectItem->setData(Qt::UserRole + 1, ui->stewEffect_effectCombo->
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

    case 18: {   /* Copy name */
        if (!root.contains("source"))
            return;

        auto source = root.value("source").toString();
        if (source != "block_entity")
            return;

        break;
    }
    }

    if (root.contains("conditions")) {
        QJsonArray conditions = root.value("conditions").toArray();
        Glhp::loadJsonToObjectsToLayout<MCRPredCondition>(conditions,
                                                          conditionsLayout);
    }
}

void LootTableFunction::onTypeChanged(int index) {
    const int maxIndex = ui->stackedWidget->count() - 1;

    if (index > maxIndex)
        ui->stackedWidget->setCurrentIndex(maxIndex);
    else
        ui->stackedWidget->setCurrentIndex(index);
}

void LootTableFunction::onAddCondition() {
    auto *cond = new MCRPredCondition(ui->conditionsContainer);

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

    if (!ui->enchantRand_list->findItems(enchantmentText, nullptr).isEmpty())
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
    amountItem->setData(Qt::DisplayRole,
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

void LootTableFunction::entries_onAdded() {
    auto *entry =
        new LootTableEntry(ui->entriesContainer);

    entriesLayout.addWidget(entry, 0);
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

void LootTableFunction::initBlocksModel() {
    auto blocksInfo = MainWindow::getMCRInfo("block");

    blocksModel.appendRow(new QStandardItem(tr("(not set)")));
    for (const auto &key : blocksInfo.keys()) {
        MCRInvItem invItem(key);
        auto      *item = new QStandardItem();
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
