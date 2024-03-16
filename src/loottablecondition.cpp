#include "loottablecondition.h"
#include "ui_loottablecondition.h"

#include "mainwindow.h"
#include "inventoryitem.h"
#include "numberproviderdelegate.h"
#include "itemconditiondialog.h"
#include "locationconditiondialog.h"
#include "entityconditiondialog.h"
#include "gameinfomodel.h"

#include "globalhelpers.h"
#include "game.h"

#include <QDialog>
#include <QDoubleSpinBox>
#include <QCompleter>


LootTableCondition::LootTableCondition(QWidget *parent) :
    QTabWidget(parent), ui(new Ui::LootTableCondition) {
    ui->setupUi(this);
}

LootTableCondition::~LootTableCondition() {
    delete ui;
}

void LootTableCondition::init() {
    if (Game::version() < Game::v1_17) {
        qobject_cast<QListView *>(ui->conditionTypeCombo->view())
        ->setRowHidden(15, true);
        static_cast<QStandardItemModel *>(ui->conditionTypeCombo->model())->item(
            15, 0)->setEnabled(false);
    }
    setTabEnabled(1, false);

    connect(ui->conditionTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &LootTableCondition::onTypeChanged);
    connect(this, &QTabWidget::currentChanged,
            this, &LootTableCondition::onTabChanged);
    connect(ui->nested_dataInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableCondition::updateConditionsTab);
    MainWindow  *mainWin = nullptr;
    const auto &&widgets = qApp->topLevelWidgets();
    for (auto *wid : widgets) {
        if (wid->objectName() == QStringLiteral("MainWindow")) {
            mainWin = qobject_cast<MainWindow *>(wid);
            break;
        }
    }
    if (mainWin) {
        connect(mainWin, &MainWindow::curDirChanged,
                this, &LootTableCondition::onCurDirChanged);
    }

    initBlockStatesPage();
    initDamageSrcPage();
    ui->entity_propBtn->assignDialogClass<EntityConditionDialog>();
    initEntityScoresPage();
    ui->matchTool_propBtn->assignDialogClass<ItemConditionDialog>();
    ui->location_propBtn->assignDialogClass<LocationConditionDialog>();
    connect(ui->nested_linkLabel, &QLabel::linkActivated, this, [this]() {
        setCurrentIndex(1);
    });
    initRandChancePage();
    m_conditionModel.setOptionalItem(false);
    m_conditionModel.setDatapackCategory("predicates", true);
    auto *completer = new QCompleter(&m_conditionModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->ref_nameEdit->setCompleter(completer);
    initTableBonusPage();
    ui->time_valueInput->setModes(NumberProvider::ExactAndRange);
    m_timeCtrl.addMapping("value", ui->time_valueInput);
    m_timeCtrl.addMapping("period", ui->time_periodSpinBox);
    initToolEnchantPage();
}

QJsonObject LootTableCondition::toJson() const {
    QJsonObject root;

    const QString &&condType = QStringLiteral("minecraft:") +
                               condTypes[ui->conditionTypeCombo->currentIndex()];

    root.insert("condition", condType);

    switch (ui->conditionTypeCombo->currentIndex()) {
        case 0: { /*Block states */
            if (!ui->blockState_slot->isEmpty()) {
                root.insert("block", ui->blockState_slot->itemNamespacedID());
            }
            QJsonObject states = LocationConditionDialog::jsonFromStateTable(
                ui->blockState_table);
            if (!states.isEmpty())
                root.insert("properties", states);
            break;
        }

        case 1: { /*Damage sources */
            QJsonObject pred;
            if (Game::version() < Game::v1_19_4) {
                ui->damageSrc_explosionCheck->insertToJsonObject(pred,
                                                                 "is_explosion");
                ui->damageSrc_projectileCheck->insertToJsonObject(pred,
                                                                  "is_projectile");
                ui->damageSrc_fireCheck->insertToJsonObject(pred, "is_fire");
                ui->damageSrc_lightningCheck->insertToJsonObject(pred,
                                                                 "is_lightning");
                ui->damageSrc_magicCheck->insertToJsonObject(pred, "is_magic");
                ui->damageSrc_starvationCheck->insertToJsonObject(pred,
                                                                  "bypasses_magic");
                ui->damageSrc_bypassArmorCheck->insertToJsonObject(pred,
                                                                   "bypasses_invulnerability");
            } else {
                pred["tags"] = ui->damageSrc_table->toJsonArray();
            }

            if (!ui->damageSrc_entityPropBtn->getData().isEmpty())
                pred.insert("source_entity",
                            ui->damageSrc_entityPropBtn->getData());
            if (!ui->damageSrc_directPropBtn->getData().isEmpty())
                pred.insert("direct_entity",
                            ui->damageSrc_directPropBtn->getData());
            if (!pred.isEmpty())
                root.insert("predicate", pred);
            break;
        }

        case 2: { /*Entity properites */
            root.insert("entity",
                        entityTargets[ui->entity_typeCombo->currentIndex()]);
            if (!ui->entity_propBtn->getData().isEmpty())
                root.insert("predicate", ui->entity_propBtn->getData());
            break;
        }

        case 3: { /*Entity scores */
            root.insert("entity",
                        entityTargets[ui->entityScores_typeCombo->currentIndex()]);
            QJsonObject scores;
            for (auto row = 0; row < ui->entityScores_table->rowCount();
                 ++row) {
                auto objective = ui->entityScores_table->item(row, 0)->text();
                auto value     = ui->entityScores_table->item(row, 1)->
                                 data(ExtendedRole::NumberProviderRole).
                                 toJsonValue();
                scores.insert(objective, value);
            }
            if (!scores.isEmpty())
                root.insert("scores", scores);
            break;
        }

        case 4: { /*Killed by player */
            if (ui->playerKill_invertedCheck->isChecked())
                root.insert("inverse", true);
            break;
        }

        case 5: { /*Location */
            if (!ui->location_xOffset->isUnset())
                root.insert("offsetX", ui->location_xOffset->value());
            if (!ui->location_yOffset->isUnset())
                root.insert("offsetY", ui->location_yOffset->value());
            if (!ui->location_zOffset->isUnset())
                root.insert("offsetZ", ui->location_zOffset->value());
            if (!ui->location_propBtn->getData().isEmpty())
                root.insert("predicate", ui->location_propBtn->getData());
            break;
        }

        case 6: { /*Nested conditions */
            QJsonArray terms;
            int        childCount = ui->nested_dataInterface->entriesCount();
            if (childCount != 0) {
                if (Game::version() < Game::v1_20) {
                    const auto &&children = ui->nested_dataInterface->json();
                    for (const auto &child : children) {
                        auto &&cond = child.toObject();
                        if (ui->nested_andRadio->isChecked())
                            addInvertCondition(cond);
                        terms.push_back(cond);
                    }
                    root.insert("terms", terms);
                    if (ui->nested_invertedCheck->isChecked()) {
                        if (ui->nested_orRadio->isChecked())
                            addInvertCondition(root);
                    } else {
                        if (ui->nested_andRadio->isChecked())
                            addInvertCondition(root);
                    }
                } else {
                    terms = ui->nested_dataInterface->json();
                    root.insert("terms", terms);
                    if (ui->nested_orRadio->isChecked()) {
                        root["condition"] = "minecraft:any_of";
                    } else {
                        root["condition"] = "minecraft:all_of";
                    }
                    root.remove("alternative");
                    if (ui->nested_invertedCheck->isChecked()) {
                        addInvertCondition(root);
                    }
                }
                auto rootMap = root.toVariantMap();
                simplifyCondition(rootMap);
                root = QJsonObject::fromVariantMap(rootMap);
            }
            break;
        }

        case 7: { /*Match tool */
            if (!ui->matchTool_propBtn->getData().isEmpty())
                root.insert("predicate", ui->matchTool_propBtn->getData());
            break;
        }

        case 8: { /*Random chance (with looting) */
            root.insert("chance", ui->randChance_spinBox->value());
            if (ui->randChance_lootingCheck->isChecked()) {
                root.insert("condition",
                            "minecraft:random_chance_with_looting");
                root.insert("looting_multiplier",
                            ui->randChance_lootingInput->value());
            }
            break;
        }

        case 9: {/*Reference */
            if (!ui->ref_nameEdit->text().isEmpty())
                root.insert("name", ui->ref_nameEdit->text());
            break;
        }

        case 10: {/*Survives Explosion */
            break;
        }

        case 11: {/*Table bonus */
            root.insert("enchantment", ui->tableBonus_enchantCombo->currentData(
                            Qt::UserRole + 1).toString());
            QJsonArray chances;
            for (int i = 0; i < tableBonusModel.rowCount(); ++i) {
                auto chance =
                    tableBonusModel.item(i,
                                         0)->data(Qt::DisplayRole).toJsonValue();
                chances.push_back(chance);
            }
            if (!chances.isEmpty())
                root.insert("chances", chances);
            break;
        }

        case 12: {/*Time */
            m_timeCtrl.putValueTo(root, {});
            break;
        }

        case 13: {/*Tool enchantment */
            QJsonArray enchantments;
            for (int i = 0; i < ui->toolEnchant_table->rowCount(); ++i) {
                auto id =
                    ui->toolEnchant_table->item(i,
                                                0)->data(Qt::UserRole +
                                                         1).toString();
                auto levels = ui->toolEnchant_table->item(i, 1)->
                              data(ExtendedRole::NumberProviderRole).toJsonValue();
                enchantments.push_back(
                    QJsonObject({ { QStringLiteral("enchantment"), id },
                                    { QStringLiteral("levels"), levels } }));
            }
            if (!enchantments.isEmpty())
                root.insert("enchantments", enchantments);
            break;
        }

        case 14: {/*Weather */
            ui->weather_rainingCheck->insertToJsonObject(root, "raining");
            ui->weather_thunderCheck->insertToJsonObject(root, "thundering");
            break;
        }

        case 15: { /* Value check */
            if (ui->value_valueInput->isCurrentlyUnset())
                break;
            if (ui->value_rangeInput->isCurrentlyUnset())
                break;
            root.insert(QLatin1String("value"), ui->value_valueInput->toJson());
            root.insert(QLatin1String("range"), ui->value_rangeInput->toJson());
            break;
        }
    }

    return root;
}

void LootTableCondition::fromJson(const QJsonObject &root, bool redirected) {
    resetAll();
    if (!root.contains(QLatin1String("condition")))
        return;

    auto valueMap = root.toVariantMap();
    simplifyCondition(valueMap);
    auto value = QJsonObject::fromVariantMap(valueMap);

    QString &&condType = value["condition"].toString();
    Glhp::removePrefix(condType, "minecraft:"_QL1);

    bool isRandChanceWithLoot = false;
    auto nestedMode           = NestedMode::Invalid;
    if (condType == "random_chance_with_looting") {
        condType             = "random_chance";
        isRandChanceWithLoot = true;
    } else if (condType == "alternative" && Game::version() < Game::v1_20) {
        condType   = "alternative";
        nestedMode = NestedMode::Alternative;
    } else if (condType == "inverted") {
        condType   = "alternative";
        nestedMode = NestedMode::SingleInvert;
    } else if (Game::version() >= Game::v1_20) {
        if (condType == "all_of") {
            condType   = "alternative";
            nestedMode = NestedMode::AllOf;
        } else if (condType == "any_of") {
            condType   = "alternative";
            nestedMode = NestedMode::AnyOf;
        }
    }

    const int condIndex = condTypes.indexOf(condType);
    if (condIndex == -1) {
        return;
    }
    const auto &&model =
        static_cast<QStandardItemModel *>(ui->conditionTypeCombo->model());
    const auto &&item = model->item(condIndex, 0);
    if (item && !item->isEnabled())
        return;

    ui->conditionTypeCombo->setCurrentIndex(condIndex);
    reset(condIndex);
    switch (condIndex) {
        case 0: { /*Block states */
            if (value.contains("block")) {
                ui->blockState_slot->setItem(value["block"].toString());
            }
            if (value.contains("properties")) {
                QJsonObject states = value["properties"].toObject();
                LocationConditionDialog::setupStateTableFromJson(
                    ui->blockState_table,
                    states);
            }

            break;
        }

        case 1: { /*Damage sources */
            if (value.contains("predicate")) {
                QJsonObject pred = value["predicate"].toObject();
                if (Game::version() < Game::v1_19_4) {
                    ui->damageSrc_explosionCheck->setupFromJsonObject(pred,
                                                                      "is_explosion");
                    ui->damageSrc_projectileCheck->setupFromJsonObject(pred,
                                                                       "is_projectile");
                    ui->damageSrc_fireCheck->setupFromJsonObject(pred,
                                                                 "is_fire");
                    ui->damageSrc_lightningCheck->setupFromJsonObject(pred,
                                                                      "is_lightning");
                    ui->damageSrc_magicCheck->setupFromJsonObject(pred,
                                                                  "is_magic");
                    ui->damageSrc_starvationCheck->setupFromJsonObject(pred,
                                                                       "bypasses_magic");
                    ui->damageSrc_bypassArmorCheck->setupFromJsonObject(pred,
                                                                        "bypasses_invulnerability");
                } else {
                    ui->damageSrc_table->fromJson(pred["tags"].toArray());
                }

                if (pred.contains("source_entity"))
                    ui->damageSrc_entityPropBtn->setData(
                        pred["source_entity"].toObject());
                if (pred.contains("direct_entity"))
                    ui->damageSrc_directPropBtn->setData(
                        pred["direct_entity"].toObject());
            }
            break;
        }

        case 2: { /*Entity properites */
            if (value.contains("entity"))
                ui->entity_typeCombo->setCurrentIndex
                    (entityTargets.indexOf(value["entity"].toString()));

            if (value.contains("predicate"))
                ui->entity_propBtn->setData(value["predicate"].toObject());
            break;
        }

        case 3: { /*Entity scores */
            if (value.contains("entity"))
                ui->entityScores_typeCombo->setCurrentIndex
                    (entityTargets.indexOf(value["entity"].toString()));
            if (value.contains("scores")) {
                const QJsonObject &scores     = value["scores"].toObject();
                const auto       &&objectives = scores.keys();
                for (const auto &objective : objectives) {
                    auto *objectiveItem =
                        new QTableWidgetItem(objective);
                    auto *valueItem = new QTableWidgetItem();
                    valueItem->setData(ExtendedRole::NumberProviderRole,
                                       scores[objective].toVariant());
                    appendRowToTableWidget(ui->entityScores_table,
                                           { objectiveItem, valueItem });
                }
            }
            break;
        }

        case 4: { /*Killed by player */
            if (value.contains("inverse"))
                ui->playerKill_invertedCheck->setChecked(
                    value["inverse"].toBool());
            break;
        }

        case 5: { /*Location */
            if (value.contains("offsetX"))
                ui->location_xOffset->setValue(value["offsetX"].toInt());
            if (value.contains("offsetY"))
                ui->location_yOffset->setValue(value["offsetY"].toInt());
            if (value.contains("offsetZ"))
                ui->location_zOffset->setValue(value["offsetZ"].toInt());
            if (value.contains("predicate"))
                ui->location_propBtn->setData(value["predicate"].toObject());
            break;
        }

        case 6: { /*Nested conditions */
            if (nestedMode == NestedMode::Invalid) {
                return;
            }
            if (!ui->nested_dataInterface->mainWidget())
                initNestedCondPage();
            if (nestedMode == NestedMode::SingleInvert
                && value.contains("term")) {
                ui->nested_dataInterface->setJson({ value["term"].toObject() });
                ui->nested_orRadio->setChecked(true);
                ui->nested_invertedCheck->setChecked(true);
            } else if (value.contains("terms")) {
                QJsonArray terms            = value["terms"].toArray();
                bool       areTermsInverted = true;
                for (const auto termRef : terms) {
                    const QJsonObject &&term = termRef.toObject();
                    if (!(term.contains("condition")
                          && term["condition"].toString().endsWith("inverted")))
                        areTermsInverted = false;
                }
                /*qDebug() << "areTermsInverted" << areTermsInverted; */

                for (QJsonValueRef && termRef : terms) {
                    QJsonObject &&term = termRef.toObject();

                    if (!term.contains("condition"))
                        continue;
                    if (areTermsInverted) {
                        auto sub = term["term"].toObject();
                        if (!sub.isEmpty())
                            term = sub;
                    }

                    termRef = term;
                }

                ui->nested_dataInterface->setJson(terms);

                if (redirected) {
                    qDebug() << "redirected";
                    if (areTermsInverted) {
                        ui->nested_andRadio->setChecked(true);
                        ui->nested_invertedCheck->setChecked(false);
                    } else {
                        ui->nested_orRadio->setChecked(true);
                        ui->nested_invertedCheck->setChecked(true);
                    }
                } else {
                    if (areTermsInverted || nestedMode == NestedMode::AllOf) {
                        ui->nested_andRadio->setChecked(true);
                        ui->nested_invertedCheck->setChecked(false);
                    } else {
                        ui->nested_orRadio->setChecked(true);
                        ui->nested_invertedCheck->setChecked(false);
                    }
                }
            }
            break;
        }

        case 7: { /*Match tool */
            if (value.contains("predicate"))
                ui->matchTool_propBtn->setData(value["predicate"].toObject());
            break;
        }

        case 8: { /*Random chance (with looting) */
            if (value.contains("chance"))
                ui->randChance_spinBox->setValue(value["chance"].toDouble());
            ui->randChance_lootingCheck->setChecked(isRandChanceWithLoot);
            if (isRandChanceWithLoot && value.contains("looting_multiplier"))
                ui->randChance_lootingInput->setValue(
                    value["looting_multiplier"].toDouble());
            break;
        }

        case 9: {/*Reference */
            if (value.contains("name"))
                ui->ref_nameEdit->setText(value["name"].toString());
            break;
        }

        case 10: {/*Survives explosion */
            break;
        }

        case 11: {/*Table bonus */
            if (value.contains("enchantment"))
                setComboValueFrom(ui->tableBonus_enchantCombo,
                                  value["enchantment"].toString());
            if (value.contains("chances")) {
                QJsonArray chances = value["chances"].toArray();
                for (auto chanceRef : chances) {
                    auto *chanceItem = new QStandardItem();
                    chanceItem->setData(chanceRef.toDouble(), Qt::DisplayRole);
                    tableBonusModel.appendRow(chanceItem);
                }
            }
            break;
        }

        case 12: {/*Time */
            m_timeCtrl.setValueFrom(root, {});
            break;
        }

        case 13: {/*Tool enchantment */
            if (value.contains("enchantments")) {
                QJsonArray enchantments = value["enchantments"].toArray();
                for (auto enchantment : enchantments) {
                    auto enchantObj = enchantment.toObject();
                    if (enchantObj.isEmpty()) continue;
                    if (!enchantObj.contains(QStringLiteral("enchantment"))) {
                        continue;
                    }
                    auto enchantId =
                        enchantObj[QStringLiteral("enchantment")].toString();
                    if (!enchantId.contains(QStringLiteral(":")))
                        enchantId = QStringLiteral("minecraft:") + enchantId;
                    auto indexes = m_enchantmentModel.match(
                        m_enchantmentModel.index(0, 0),
                        Qt::UserRole + 1,
                        enchantId);
                    if (indexes.isEmpty()) continue;
                    auto *enchantItem = new QTableWidgetItem();
                    enchantItem->setData(Qt::UserRole + 1, enchantId);
                    enchantItem->setText(indexes[0].data(
                                             Qt::DisplayRole).toString());
                    enchantItem->setFlags(enchantItem->flags() &
                                          ~Qt::ItemIsEditable);
                    auto *levelsItem = new QTableWidgetItem();
                    levelsItem->setData(ExtendedRole::NumberProviderRole,
                                        enchantObj.value(QStringLiteral(
                                                             "levels")));
                    appendRowToTableWidget(ui->toolEnchant_table,
                                           { enchantItem, levelsItem });
                }
            }
            break;
        }

        case 14: {/*Weather */
            ui->weather_rainingCheck->setupFromJsonObject(value, "raining");
            ui->weather_thunderCheck->setupFromJsonObject(value, "thundering");
            break;
        }

        case 15: { /* Value check */
            if (Game::version() >= Game::v1_17) {
                if (value.contains(QLatin1String("value")))
                    ui->value_valueInput->fromJson(value[QLatin1String("value")]);
                if (value.contains(QLatin1String("range")))
                    ui->value_rangeInput->fromJson(value[QLatin1String("range")]);
            }
            break;
        }

        default:
            break;
    }
}

void LootTableCondition::onTypeChanged(const int i) {
    ui->stackedWidget->setCurrentIndex(i);
    const int nestedConditionIndex = 6;
    setTabEnabled(1, i == nestedConditionIndex);
}

void LootTableCondition::onTabChanged(const int i) {
    if ((i == 1) && !ui->nested_dataInterface->mainWidget())
        initNestedCondPage();
}

void LootTableCondition::onCurDirChanged(const QDir &dir) {
    Q_UNUSED(dir)
    m_conditionModel.setDatapackCategory("predicates", true);
}

void LootTableCondition::updateConditionsTab(int size) {
    setTabText(1, tr("Conditions (%1)").arg(size));
}

void LootTableCondition::reset(int index) {
    switch (index) {
        case 0: { /*Block states */
            ui->blockState_slot->clearItems();
            ui->blockState_table->setRowCount(0);
            break;
        }

        case 1: { /*Damage sources */
            ui->damageSrc_explosionCheck->unset();
            ui->damageSrc_projectileCheck->unset();
            ui->damageSrc_fireCheck->unset();
            ui->damageSrc_lightningCheck->unset();
            ui->damageSrc_magicCheck->unset();
            ui->damageSrc_starvationCheck->unset();
            ui->damageSrc_bypassArmorCheck->unset();

            ui->damageSrc_entityPropBtn->reset();
            ui->damageSrc_directPropBtn->reset();
            break;
        }

        case 2: { /*Entity properites */
            ui->entity_typeCombo->setCurrentIndex(0);
            ui->entity_propBtn->reset();
            break;
        }

        case 3: { /*Entity scores */
            ui->entityScores_typeCombo->setCurrentIndex(0);
            ui->entityScores_table->setRowCount(0);
            break;
        }

        case 4: { /*Killed by player */
            ui->playerKill_invertedCheck->setChecked(false);
            break;
        }

        case 5: { /*Location */
            ui->location_xOffset->unset();
            ui->location_yOffset->unset();
            ui->location_zOffset->unset();

            ui->location_propBtn->reset();
            break;
        }

        case 6: { /*Nested conditions */
            if (ui->nested_dataInterface->mainWidget())
                ui->nested_dataInterface->setJson({});
            break;
        }

        case 7: { /*Match tool */
            ui->matchTool_propBtn->reset();
            break;
        }

        case 8: { /*Random chance (with looting) */
            ui->randChance_spinBox->setValue(0);
            ui->randChance_lootingCheck->setChecked(false);
            ui->randChance_lootingInput->setValue(1);
            break;
        }

        case 9: {/*Reference */
            ui->ref_nameEdit->setText("");
            break;
        }

        case 10: {/*Survives Explosion */
            break;
        }

        case 11: {/*Table bonus */
            ui->tableBonus_enchantCombo->setCurrentIndex(0);
            clearModelExceptHeaders(tableBonusModel);
            break;
        }

        case 12: {/*Time */
            ui->time_valueInput->unset();
            ui->time_periodSpinBox->setValue(0);
            break;
        }

        case 13: {/*Tool enchantment */
            ui->toolEnchant_table->setRowCount(0);
            break;
        }

        case 14: {/*Weather */
            ui->weather_rainingCheck->unset();
            ui->weather_thunderCheck->unset();
            break;
        }

        case 15: { /* Value check */
            ui->value_valueInput->unset();
            ui->value_rangeInput->unset();
            break;
        }

        default:
            break;
    }
}

void LootTableCondition::clearModelExceptHeaders(QStandardItemModel &model) {
    QVector<QStandardItem *> headers;

    for (int i = 0; i < model.columnCount(); ++i) {
        headers << model.takeHorizontalHeaderItem(i);
    }
    model.clear();
    for (int i = 0; i < headers.count(); ++i)
        model.setHorizontalHeaderItem(i, headers[i]);
}

void LootTableCondition::setDepth(int value) {
    depth = value;
}

void LootTableCondition::resetAll() {
    std::call_once(m_fullyInitialized, &LootTableCondition::init, this);
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
        reset(i);
    ui->conditionTypeCombo->setCurrentIndex(0);
}

void LootTableCondition::changeEvent(QEvent *event) {
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void LootTableCondition::showEvent(QShowEvent *event) {
    std::call_once(m_fullyInitialized, &LootTableCondition::init, this);
    QTabWidget::showEvent(event);
}

void LootTableCondition::blockStates_onAdded() {
    if (ui->blockState_stateEdit->text().isEmpty()
        || ui->blockState_valueEdit->text().isEmpty())
        return;

    QTableWidgetItem *stateItem = new QTableWidgetItem(
        ui->blockState_stateEdit->text());
    QTableWidgetItem *valueItem = new QTableWidgetItem(
        ui->blockState_valueEdit->text());

    appendRowToTableWidget(ui->blockState_table, { stateItem, valueItem });
}

void LootTableCondition::entityScores_onAdded() {
    if (ui->entityScores_objectiveEdit->text().isEmpty())
        return;

    auto *objItem = new QTableWidgetItem(
        ui->entityScores_objectiveEdit->text());
    auto *valueItem = new QTableWidgetItem();
    auto  json      = ui->entityScores_valueInput->toJson();
    valueItem->setData(ExtendedRole::NumberProviderRole, json);
    appendRowToTableWidget(ui->entityScores_table, { objItem, valueItem });
}

void LootTableCondition::tableBonus_onAdded() {
    QStandardItem *chanceItem = new QStandardItem();

    chanceItem->setData(ui->tableBonus_chanceSpinBox->value(),
                        Qt::DisplayRole);
    tableBonusModel.appendRow(chanceItem);
}

void LootTableCondition::toolEnchant_onAdded() {
    if (ui->toolEnchant_levelsInput->minValue() == 0
        || ui->toolEnchant_levelsInput->maxValue() == 0) {
        return;
    }
    QString enchantmentText = ui->toolEnchant_enchantCombo->currentText();
    if (!ui->toolEnchant_table->findItems(enchantmentText,
                                          Qt::MatchExactly).isEmpty())
        return;

    QTableWidgetItem *enchantItem = new QTableWidgetItem(enchantmentText);
    enchantItem->setData(Qt::UserRole + 1, ui->toolEnchant_enchantCombo->
                         currentData(Qt::UserRole + 1));
    enchantItem->setFlags(enchantItem->flags() & ~Qt::ItemIsEditable);
    QTableWidgetItem *levelsItem = new QTableWidgetItem();
    auto              json       = ui->toolEnchant_levelsInput->toJson();
    levelsItem->setData(ExtendedRole::NumberProviderRole, json);
    appendRowToTableWidget(ui->toolEnchant_table, { enchantItem, levelsItem });
}

void LootTableCondition::initBlockStatesPage() {
    ui->blockState_slot->setAcceptMultiple(false);
    ui->blockState_slot->setAcceptTag(false);
    ui->blockState_slot->setSelectCategory(InventorySlot::SelectCategory::Blocks);

    ui->blockState_table->installEventFilter(&viewFilter);

    connect(ui->blockState_addBtn, &QPushButton::clicked,
            this, &LootTableCondition::blockStates_onAdded);
}

void LootTableCondition::initDamageSrcPage() {
    if (Game::version() < Game::v1_19_4) {
        ui->damageSrc_stackWidget->setCurrentIndex(0);
    } else {
        ui->damageSrc_stackWidget->setCurrentIndex(1);
        m_damageTagModel.setInfo(QStringLiteral("tag/damage_type"),
                                 GameInfoModel::PrependPrefix |
                                 GameInfoModel::ForceKeyAsName);
        m_damageTagModel.setDatapackCategory("tags/damage_type", true);
        ui->damageSrc_tagEdit->setCompleter(m_damageTagModel.createCompleter());
        ui->damageSrc_table->appendColumnMapping("id", ui->damageSrc_tagEdit);
        ui->damageSrc_table->appendColumnMapping("expected",
                                                 ui->damageSrc_expectedCheck);
    }
    ui->damageSrc_entityPropBtn->assignDialogClass<EntityConditionDialog>();
    ui->damageSrc_directPropBtn->assignDialogClass<EntityConditionDialog>();
}

void LootTableCondition::initEntityScoresPage() {
    ui->entityScores_valueInput->setModes(
        NumberProvider::ExactAndRange);

    auto *delegate = new NumberProviderDelegate(this);
    delegate->setInputModes(NumberProvider::ExactAndRange);

    ui->entityScores_table->setItemDelegate(delegate);
    ui->entityScores_table->installEventFilter(new ViewEventFilter(this));

    connect(ui->entityScores_addBtn, &QPushButton::clicked,
            this, &LootTableCondition::entityScores_onAdded);
}

void LootTableCondition::initNestedCondPage() {
    auto *cond = new LootTableCondition();

    /*cond->setMinimumHeight(500); */
    /*cond->setIsModular(true); */
    ui->nested_dataInterface->setMainWidget(cond);

    ui->nested_dataInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&LootTableCondition::fromJson));

    ui->nested_dataInterface->mapToGetter(&LootTableCondition::toJson, cond);
}

void LootTableCondition::initRandChancePage() {
    connect(ui->randChance_slider, &QSlider::valueChanged,
            this, [ = ](int value) {
        ui->randChance_spinBox->setValue((double)value / 100);
    });
    connect(ui->randChance_spinBox,
            qOverload<double>(&QDoubleSpinBox::valueChanged), this,
            [ = ](double value) {
        ui->randChance_slider->setValue(qRound(value * 100));
    });
    connect(ui->randChance_lootingCheck,
            &QCheckBox::toggled, this, [ = ](bool checked) {
        ui->randChance_lootingLabel->setEnabled(checked);
        ui->randChance_lootingInput->setEnabled(checked);
    });
}

void LootTableCondition::initTableBonusPage() {
    m_enchantmentModel.setInfo(QStringLiteral("enchantment"),
                               GameInfoModel::PrependPrefix);
    ui->tableBonus_enchantCombo->setModel(&m_enchantmentModel);
    ui->toolEnchant_enchantCombo->setModel(&m_enchantmentModel);

    ui->tableBonus_listView->setModel(&tableBonusModel);
    ui->tableBonus_listView->installEventFilter(&viewFilter);

    connect(ui->tableBonus_chanceSlider,
            &QSlider::valueChanged, this, [ = ](int value) {
        ui->tableBonus_chanceSpinBox->setValue((double)value / 100);
    });
    connect(ui->tableBonus_chanceSpinBox,
            qOverload<double>(&QDoubleSpinBox::valueChanged), this,
            [ = ](double value) {
        ui->tableBonus_chanceSlider->setValue(qRound(value * 100));
    });
    connect(ui->tableBonus_addBtn, &QPushButton::clicked,
            this, &LootTableCondition::tableBonus_onAdded);
}

void LootTableCondition::initToolEnchantPage() {
    ui->toolEnchant_levelsInput->setModes(NumberProvider::Range);

    auto *delegate = new NumberProviderDelegate(this);
    delegate->setInputModes(NumberProvider::ExactAndRange);
    ui->toolEnchant_table->setItemDelegate(delegate);
    ui->toolEnchant_table->installEventFilter(&viewFilter);

    connect(ui->toolEnchant_addBtn, &QPushButton::clicked,
            this, &LootTableCondition::toolEnchant_onAdded);
}

void LootTableCondition::addInvertCondition(QJsonObject &json) const {
    json = QJsonObject({ { "condition", "minecraft:inverted" },
                           { "term", json } });
}

void LootTableCondition::simplifyCondition(QVariantMap &condMap,
                                           int depth) const {
    const QString anyOfString =
        (Game::version() < Game::v1_20) ? "alternative" : "any_of";

    /*const QString tab = QString(" ").repeated(depth); */

    /*qDebug().noquote() << tab << "simplifyCondJson" << depth; */
    if (condMap.contains("condition")) {
        /*
           qDebug().noquote() << tab << "main condition:" <<
           condMap["condition"].toString();
         */
        if (condMap["condition"].toString().endsWith("inverted")) {
            auto &subRef = condMap["term"];
            auto  sub    = subRef.toMap();
            if (sub.contains("condition")) {
                /*qDebug().noquote() << tab << "sub condition:" << */
                sub["condition"].toString();
                if (sub["condition"].toString().endsWith("inverted")) {
                    auto subTerm = sub["term"].toMap();
                    condMap = subTerm;
                    simplifyCondition(condMap, depth + 2);
                } else if (sub["condition"].toString().endsWith(anyOfString)) {
                    auto &subTermsRef = sub["terms"];
                    auto  subTerms    = subTermsRef.toList();
                    /*qDebug().noquote() << tab << "sub terms count:" << */
                    subTerms.count();
                    if (subTerms.count() > 1) {
                        for (auto &termRef : subTerms) {
                            auto term = termRef.toMap();
                            simplifyCondition(term, depth + 2);
                            termRef.setValue(term);
                        }
                        subTermsRef.setValue(subTerms);
                        subRef.setValue(sub);
                    } else if (subTerms.count() == 1) {
                        auto term = subTerms[0].toMap();
                        sub = term;
                        subRef.setValue(sub);
                        simplifyCondition(condMap, depth + 1);
                    }
                }
            }
        } else if (condMap["condition"].toString().endsWith(anyOfString)) {
            auto &subsRef = condMap["terms"];
            auto  subs    = subsRef.toList();
            /*qDebug().noquote() << tab << "subs count:" << subs.count(); */
            if (subs.count() > 1) {
                for (auto &termRef : subs) {
                    auto term = termRef.toMap();
                    simplifyCondition(term, depth + 1);
                    termRef.setValue(term);
                }
                subsRef.setValue(subs);
            } else if (subs.count() == 1) {
                auto term = subs[0].toMap();
                condMap = term;
                simplifyCondition(condMap, depth + 1);
            }
        }
    }
    /*qDebug().noquote() << tab << "end" << depth; */
}
