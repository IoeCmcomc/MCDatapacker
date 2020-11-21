#include "mcrpredcondition.h"
#include "ui_mcrpredcondition.h"

#include "mainwindow.h"
#include "mcrinvitem.h"
#include "extendeddelegate.h"
#include "itemconditiondialog.h"
#include "locationconditiondialog.h"
#include "entityconditiondialog.h"
#include "globalhelpers.h"

#include <QDialog>
#include <QDoubleSpinBox>

MCRPredCondition::MCRPredCondition(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MCRPredCondition) {
    ui->setupUi(this);

    setIsModular(true);

    connect(ui->deleteButton, &QPushButton::clicked, this,
            &QObject::deleteLater);
    connect(ui->conditionTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &MCRPredCondition::onTypeChanged);
    MainWindow *mainWin;
    for (auto wid : qApp->topLevelWidgets()) {
        if (wid->objectName() == QStringLiteral("MainWindow")) {
            mainWin = qobject_cast<MainWindow*>(wid);
            break;
        }
    }
    if (mainWin) {
        connect(mainWin, &MainWindow::curDirChanged,
                this, &MCRPredCondition::onCurDirChanged);
    }
    connect(&predRefWatcher, &QFileSystemWatcher::directoryChanged,
            this, &MCRPredCondition::setupRefCombo);

    initBlockStatesPage();
    ui->damageSrc_entityPropBtn->setDialogType(DDBType::EntityCond);
    ui->damageSrc_directPropBtn->setDialogType(DDBType::EntityCond);
    ui->entity_propBtn->setDialogType(DDBType::EntityCond);
    initEntityScoresPage();
    initInvertedCondPage();
    initNestedCondPage();
    ui->matchTool_propBtn->setDialogType(DDBType::ItemCond);
    ui->location_propBtn->setDialogType(DDBType::LocationCond);
    initRandChancePage();
    setupRefCombo();
    initTableBonusPage();
    ui->time_valueInput->setTypes(NumericInput::Exact | NumericInput::Range);
    initToolEnchantPage();
}

MCRPredCondition::~MCRPredCondition() {
    delete ui;
}

bool MCRPredCondition::getIsModular() const {
    return isModular;
}

void MCRPredCondition::setIsModular(bool value) {
    isModular = value;
    ui->deleteButton->setVisible(isModular);
    setFrameShape((isModular) ? QFrame::StyledPanel : QFrame::NoFrame);
    setFrameShadow((isModular) ? QFrame::Raised : QFrame::Plain);
}

QJsonObject MCRPredCondition::toJson() const {
    QJsonObject root;

    root.insert("condition", "minecraft:" +
                condTypes[ui->conditionTypeCombo->currentIndex()]);

    switch (ui->conditionTypeCombo->currentIndex()) {
    case 0: { /*Block states */
        if (ui->blockState_blockCombo->currentIndex() != 0) {
            auto invItem = ui->blockState_blockCombo->currentData(
                Qt::UserRole + 1).value<MCRInvItem>();
            root.insert("block", invItem.getNamespacedID());
        }
        QJsonObject states = LocationConditionDialog::jsonFromStateTable(
            ui->blockState_table);
        if (!states.isEmpty())
            root.insert("properties", states);
        break;
    }

    case 1: { /*Damage sources */
        QJsonObject pred;
        ui->damageSrc_explosionCheck->insertToJsonObject(pred, "is_explosion");
        ui->damageSrc_projectileCheck->insertToJsonObject(pred,
                                                          "is_projectile");
        ui->damageSrc_fireCheck->insertToJsonObject(pred, "is_fire");
        ui->damageSrc_lightningCheck->insertToJsonObject(pred, "is_lightning");
        ui->damageSrc_magicCheck->insertToJsonObject(pred, "is_magic");
        ui->damageSrc_starvationCheck->insertToJsonObject(pred,
                                                          "bypasses_magic");
        ui->damageSrc_bypassArmorCheck->insertToJsonObject(pred,
                                                           "bypasses_invulnerability");

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
        for (auto row = 0; row < ui->entityScores_table->rowCount(); ++row) {
            auto objective = ui->entityScores_table->item(row, 0)->text();
            auto value     = ui->entityScores_table->item(row, 1)->
                             data(Qt::DisplayRole).toJsonValue();
            scores.insert(objective, value);
        }
        if (!scores.isEmpty())
            root.insert("scores", scores);
        break;
    }

    case 4: { /*Inverted */
        MCRPredCondition *cond =
            ui->inverted_condAreaInner->findChild<MCRPredCondition*>();
        if (cond != nullptr)
            root.insert("term", cond->toJson());
        break;
    }

    case 5: { /*Killed by player */
        if (ui->playerKill_invertedCheck->isChecked())
            root.insert("inverse", true);
        break;
    }

    case 6: { /*Location */
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

    case 7: { /*Nested conditions */
        QJsonArray terms;
        int        childCount = ui->nested_condAreaInner->children().count();
        if (childCount != 0) {
            for (auto *child : ui->nested_condAreaInner->children()) {
                MCRPredCondition *childCond = qobject_cast<MCRPredCondition*>(
                    child);
                if (childCond != nullptr) {
                    QJsonObject cond = childCond->toJson();
                    if (ui->nested_andRadio->isChecked())
                        addInvertCondition(cond);
                    terms.push_back(cond);
                }
            }
            root.insert("terms", terms);
            if (ui->nested_invertedCheck->isChecked()) {
                if (ui->nested_orRadio->isChecked())
                    addInvertCondition(root);
            } else {
                if (ui->nested_andRadio->isChecked())
                    addInvertCondition(root);
            }
            auto rootMap = root.toVariantMap();
            simplifyCondition(rootMap);
            root = root.fromVariantMap(rootMap);
        }
        break;
    }

    case 8: { /*Match tool */
        if (!ui->matchTool_propBtn->getData().isEmpty())
            root.insert("predicate", ui->matchTool_propBtn->getData());
        break;
    }

    case 9: { /*Random chance (with looting) */
        root.insert("chance", ui->randChance_spinBox->value());
        if (ui->randChance_lootingCheck->isChecked()) {
            root.insert("condition", "minecraft:random_chance_with_looting");
            root.insert("looting_multiplier",
                        ui->randChance_lootingInput->value());
        }
        break;
    }

    case 10: {/*Reference */
        if (!ui->ref_nameCombo->currentText().isEmpty())
            root.insert("name", ui->ref_nameCombo->currentText());
        break;
    }

    case 11: {/*Survives Explosion */
        break;
    }

    case 12: {/*Table bonus */
        root.insert("enchantment", ui->tableBonus_enchantCombo->currentData(
                        Qt::UserRole + 1).toString());
        QJsonArray chances;
        for (int i = 0; i < tableBonusModel.rowCount(); ++i) {
            auto chance =
                tableBonusModel.item(i, 0)->data(Qt::DisplayRole).toJsonValue();
            chances.push_back(chance);
        }
        if (!chances.isEmpty())
            root.insert("chances", chances);
        break;
    }

    case 13: {/*Time */
        root.insert("value", ui->time_valueInput->toJson());
        if (ui->time_periodSpinBox->value() > 0)
            root.insert("period", ui->time_periodSpinBox->value());
        break;
    }

    case 14: {/*Tool enchantment */
        QJsonArray enchantments;
        for (int i = 0; i < ui->toolEnchant_table->rowCount(); ++i) {
            auto id =
                ui->toolEnchant_table->item(i,
                                            0)->data(Qt::UserRole +
                                                     1).toString();
            auto levels = ui->toolEnchant_table->item(i, 1)->
                          data(Qt::DisplayRole).toJsonValue();
            enchantments.push_back(
                QJsonObject({ { QStringLiteral("enchantment"), id },
                                { QStringLiteral("levels"), levels } }));
        }
        if (!enchantments.isEmpty())
            root.insert("enchantments", enchantments);
        break;
    }

    case 15: {/*Weather */
        ui->weather_rainingCheck->insertToJsonObject(root, "raining");
        ui->weather_thunderCheck->insertToJsonObject(root, "thundering");
        break;
    }
    }

    return root;
}

void MCRPredCondition::fromJson(const QJsonObject &root, bool redirected) {
    if (!root.contains("condition"))
        return;

    auto valueMap = root.toVariantMap();
    simplifyCondition(valueMap);
    auto value = QJsonObject::fromVariantMap(valueMap);

    QString condType = value["condition"].toString();
    Glhp::removePrefix(condType, "minecraft:");

    bool isRandChanceWithLoot = false;
    if (condType.endsWith("random_chance_with_looting")) {
        condType             = "random_chance";
        isRandChanceWithLoot = true;
    }

    int condIndex = condTypes.indexOf(condType);
    ui->conditionTypeCombo->setCurrentIndex(condIndex);
    reset(condIndex);
    switch (condIndex) {
    case 0: { /*Block states */
        if (value.contains("block"))
            setupComboFrom(ui->blockState_blockCombo, QVariant::fromValue
                               (MCRInvItem(value["block"].toString())));
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
            ui->damageSrc_explosionCheck->setupFromJsonObject(pred,
                                                              "is_explosion");
            ui->damageSrc_projectileCheck->setupFromJsonObject(pred,
                                                               "is_projectile");
            ui->damageSrc_fireCheck->setupFromJsonObject(pred, "is_fire");
            ui->damageSrc_lightningCheck->setupFromJsonObject(pred,
                                                              "is_lightning");
            ui->damageSrc_magicCheck->setupFromJsonObject(pred, "is_magic");
            ui->damageSrc_starvationCheck->setupFromJsonObject(pred,
                                                               "bypasses_magic");
            ui->damageSrc_bypassArmorCheck->setupFromJsonObject(pred,
                                                                "bypasses_invulnerability");

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
            QJsonObject scores = value["scores"].toObject();
            for (auto objective : scores.keys()) {
                QTableWidgetItem *objectiveItem =
                    new QTableWidgetItem(objective);
                QTableWidgetItem *valueItem = new QTableWidgetItem();
                valueItem->setData(Qt::DisplayRole,
                                   scores[objective].toVariant());
                appendRowToTableWidget(ui->entityScores_table,
                                       { objectiveItem, valueItem });
            }
        }
        break;
    }

    case 4: { /*Inverted */
        if (value.contains("term")) {
            QJsonObject term = value["term"].toObject();
            if (!term.contains("condition"))
                return;

            if (term["condition"].toString().endsWith("alternative")) {
                fromJson(term, true);
            } else {
                MCRPredCondition *cond =
                    ui->inverted_condAreaInner->findChild<MCRPredCondition*>();
                if (cond == nullptr) {
                    cond = new MCRPredCondition(ui->nested_condAreaInner);
                    cond->setMinimumHeight(300);
                    invertedCondLayout.addWidget(cond, 0);
                }
                cond->fromJson(term);
            }
        }
        break;
    }

    case 5: { /*Killed by player */
        if (value.contains("inverse"))
            ui->playerKill_invertedCheck->setChecked(value["inverse"].toBool());
        break;
    }

    case 6: { /*Location */
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

    case 7: { /*Nested conditions */
        if (value.contains("terms")) {
            QJsonArray terms            = value["terms"].toArray();
            bool       areTermsInverted = true;
            for (auto termRef : terms) {
                QJsonObject term = termRef.toObject();
                if (!(term.contains("condition")
                      && term["condition"].toString().endsWith("inverted")))
                    areTermsInverted = false;
            }
            qDebug() << "areTermsInverted" << areTermsInverted;

            for (auto termRef : terms) {
                QJsonObject term = termRef.toObject();

                if (!term.contains("condition"))
                    continue;
                if (areTermsInverted) {
                    auto sub = term["term"].toObject();
                    if (!sub.isEmpty())
                        term = sub;
                }

                MCRPredCondition *cond = new MCRPredCondition(
                    ui->nested_condAreaInner);
                cond->setMinimumHeight(300);
                nestedCondLayout.addWidget(cond, 0);
                cond->fromJson(term);
            }

            if (redirected) {
                if (areTermsInverted) {
                    ui->nested_andRadio->setChecked(true);
                    ui->nested_invertedCheck->setChecked(false);
                } else {
                    ui->nested_orRadio->setChecked(true);
                    ui->nested_invertedCheck->setChecked(true);
                }
            } else {
                if (areTermsInverted) {
                    ui->nested_andRadio->setChecked(true);
                    ui->nested_invertedCheck->setChecked(true);
                } else {
                    ui->nested_orRadio->setChecked(true);
                    ui->nested_invertedCheck->setChecked(false);
                }
            }
        }
        break;
    }

    case 8: { /*Match tool */
        if (value.contains("predicate"))
            ui->matchTool_propBtn->setData(value["predicate"].toObject());
        break;
    }

    case 9: { /*Random chance (with looting) */
        if (value.contains("chance"))
            ui->randChance_spinBox->setValue(value["chance"].toDouble());
        ui->randChance_lootingCheck->setChecked(isRandChanceWithLoot);
        if (isRandChanceWithLoot && value.contains("looting_multiplier"))
            ui->randChance_lootingInput->setValue(
                value["looting_multiplier"].toDouble());
        break;
    }

    case 10: {/*Reference */
        if (value.contains("name"))
            ui->ref_nameCombo->setCurrentText(value["name"].toString());
        break;
    }

    case 11: {/*Survives explosion */
        break;
    }

    case 12: {/*Table bonus */
        if (value.contains("enchantment"))
            setupComboFrom(ui->tableBonus_enchantCombo,
                           value["enchantment"].toString());
        if (value.contains("chances")) {
            QJsonArray chances = value["chances"].toArray();
            for (auto chanceRef : chances) {
                QStandardItem *chanceItem = new QStandardItem();
                chanceItem->setData(chanceRef.toDouble(), Qt::DisplayRole);
                tableBonusModel.appendRow(chanceItem);
            }
        }
        break;
    }

    case 13: {/*Time */
        if (value.contains("value"))
            ui->time_valueInput->fromJson(value["value"]);
        if (value.contains("period") && value["period"].toInt() > 0)
            ui->time_periodSpinBox->setValue(value["period"].toInt());
        break;
    }

    case 14: {/*Tool enchantment */
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
                auto indexes = enchantmentsModel.match(
                    enchantmentsModel.index(0, 0), Qt::UserRole + 1, enchantId);
                if (indexes.isEmpty()) continue;
                QTableWidgetItem *enchantItem = new QTableWidgetItem();
                enchantItem->setData(Qt::UserRole + 1, enchantId);
                enchantItem->setText(indexes[0].data(Qt::DisplayRole).toString());
                enchantItem->setFlags(enchantItem->flags() &
                                      ~Qt::ItemIsEditable);
                QTableWidgetItem *levelsItem = new QTableWidgetItem();
                levelsItem->setData(Qt::DisplayRole,
                                    enchantObj.value(QStringLiteral("levels")));
                appendRowToTableWidget(ui->toolEnchant_table,
                                       { enchantItem, levelsItem });
            }
        }
        break;
    }

    case 15: {/*Weather */
        ui->weather_rainingCheck->setupFromJsonObject(value, "raining");
        ui->weather_thunderCheck->setupFromJsonObject(value, "thundering");
        break;
    }

    default:
        break;
    }
}

void MCRPredCondition::onTypeChanged(const int &i) {
    ui->stackedWidget->setCurrentIndex(i);

    if ((ui->stackedWidget->currentWidget() == ui->inverted) &&
        invertedCondLayout.isEmpty()) {
        if (depth < 1) {
            MCRPredCondition *cond = new MCRPredCondition(
                ui->nested_condAreaInner);
            cond->setDepth(depth + 1);
            cond->sizeHint().rheight() = minimumHeight();
            /*cond->setIsModular(false); */
            invertedCondLayout.addWidget(cond, 0);
        } else {
            ui->inverted_condAreaInner->setText(
                tr("Inverting an inverted condition is very silly."));
        }
    }
}

void MCRPredCondition::onCurDirChanged(const QString &path) {
    if (!predRefWatcher.directories().isEmpty())
        predRefWatcher.removePaths(predRefWatcher.directories());
    predRefWatcher.addPath(path);
    predRefWatcher.directories();
    setupRefCombo();
}

void MCRPredCondition::reset(int index) {
    switch (index) {
    case 0: { /*Block states */
        ui->blockState_blockCombo->setCurrentIndex(0);
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

    case 4: { /*Inverted */
        auto *cond =
            ui->inverted_condAreaInner->findChild<MCRPredCondition*>();
        if (cond != nullptr)
            /*cond->deleteLater(); */
            delete cond;
        break;
    }

    case 5: { /*Killed by player */
        ui->playerKill_invertedCheck->setChecked(false);
        break;
    }

    case 6: { /*Location */
        ui->location_xOffset->unset();
        ui->location_yOffset->unset();
        ui->location_zOffset->unset();

        ui->location_propBtn->reset();
        break;
    }

    case 7: { /*Nested conditions */
        auto nestedCondWids =
            ui->nested_condAreaInner->findChildren<MCRPredCondition*>(
                QString(),
                Qt::FindDirectChildrenOnly);
        for (auto child : nestedCondWids) {
            child->deleteLater();
            /*delete child; */
        }
        break;
    }

    case 8: { /*Match tool */
        ui->matchTool_propBtn->reset();
        break;
    }

    case 9: { /*Random chance (with looting) */
        ui->randChance_spinBox->setValue(0);
        ui->randChance_lootingCheck->setChecked(false);
        ui->randChance_lootingInput->setValue(1);
        break;
    }

    case 10: {/*Reference */
        ui->ref_nameCombo->setCurrentText("");
        break;
    }

    case 11: {/*Survives Explosion */
        break;
    }

    case 12: {/*Table bonus */
        ui->tableBonus_enchantCombo->setCurrentIndex(0);
        clearModelExceptHeaders(tableBonusModel);
        break;
    }

    case 13: {/*Time */
        ui->time_valueInput->unset();
        ui->time_periodSpinBox->setValue(0);
        break;
    }

    case 14: {/*Tool enchantment */
        ui->toolEnchant_table->setRowCount(0);
        break;
    }

    case 15: {/*Weather */
        ui->weather_rainingCheck->unset();
        ui->weather_thunderCheck->unset();
        break;
    }

    default:
        break;
    }
}

void MCRPredCondition::clearModelExceptHeaders(QStandardItemModel &model) {
    QVector<QStandardItem*> headers;

    for (int i = 0; i < model.columnCount(); ++i) {
        headers << model.takeHorizontalHeaderItem(i);
    }
    model.clear();
    for (int i = 0; i < headers.count(); ++i)
        model.setHorizontalHeaderItem(i, headers[i]);
}

void MCRPredCondition::setDepth(int value) {
    depth = value;
}

void MCRPredCondition::resetAll() {
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
        reset(i);
}

void MCRPredCondition::changeEvent(QEvent *event) {
    QFrame::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (!ui->inverted_condAreaInner->text().isEmpty())
            ui->inverted_condAreaInner->setText(
                tr("Inverting an inverted condition is very silly."));
    }
}

void MCRPredCondition::blockStates_onAdded() {
    if (ui->blockState_stateEdit->text().isEmpty()
        || ui->blockState_valueEdit->text().isEmpty())
        return;

    QTableWidgetItem *stateItem = new QTableWidgetItem(
        ui->blockState_stateEdit->text());
    QTableWidgetItem *valueItem = new QTableWidgetItem(
        ui->blockState_valueEdit->text());

    appendRowToTableWidget(ui->blockState_table, { stateItem, valueItem });
}

void MCRPredCondition::entityScores_onAdded() {
    if (ui->entityScores_objectiveEdit->text().isEmpty())
        return;

    QTableWidgetItem *objItem = new QTableWidgetItem(
        ui->entityScores_objectiveEdit->text());
    QTableWidgetItem *valueItem = new QTableWidgetItem();
    auto              json      = ui->entityScores_valueInput->toJson();
    valueItem->setData(Qt::DisplayRole, json);
    appendRowToTableWidget(ui->entityScores_table, { objItem, valueItem });
}

void MCRPredCondition::nested_onAdded() {
    MCRPredCondition *cond = new MCRPredCondition(ui->nested_condAreaInner);

    cond->sizeHint().rheight() = minimumHeight();
    nestedCondLayout.addWidget(cond, 0);
}

void MCRPredCondition::setupRefCombo() {
    /*qDebug() << "setupRefCombo"; */
    if (condRefsModel.rowCount() > 0)
        condRefsModel.clear();
    auto predRefIDs = Glhp::fileIDList(MainWindow::getCurDir(),
                                       "predicates");

    for (auto predRef : predRefIDs)
        condRefsModel.appendRow(new QStandardItem(predRef));
    ui->ref_nameCombo->setModel(&condRefsModel);
}

void MCRPredCondition::tableBonus_onAdded() {
    QStandardItem *chanceItem = new QStandardItem();

    chanceItem->setData(ui->tableBonus_chanceSpinBox->value(),
                        Qt::DisplayRole);
    tableBonusModel.appendRow(chanceItem);
}

void MCRPredCondition::toolEnchant_onAdded() {
    if (ui->toolEnchant_levelsInput->getMinimum() == 0
        || ui->toolEnchant_levelsInput->getMaximum() == 0) {
        return;
    }
    QString enchantmentText = ui->toolEnchant_enchantCombo->currentText();
    if (!ui->toolEnchant_table->findItems(enchantmentText, 0).isEmpty())
        return;

    QTableWidgetItem *enchantItem = new QTableWidgetItem(enchantmentText);
    enchantItem->setData(Qt::UserRole + 1, ui->toolEnchant_enchantCombo->
                         currentData(Qt::UserRole + 1));
    enchantItem->setFlags(enchantItem->flags() & ~Qt::ItemIsEditable);
    QTableWidgetItem *levelsItem = new QTableWidgetItem();
    auto              json       = ui->toolEnchant_levelsInput->toJson();
    levelsItem->setData(Qt::DisplayRole, json);
    appendRowToTableWidget(ui->toolEnchant_table, { enchantItem, levelsItem });
}

void MCRPredCondition::initBlockStatesPage() {
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
    ui->blockState_blockCombo->setModel(&blocksModel);
    ui->blockState_table->installEventFilter(&viewFilter);

    connect(ui->blockState_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::blockStates_onAdded);
}

void MCRPredCondition::initEntityScoresPage() {
    ui->entityScores_valueInput->setTypes(
        NumericInput::Exact | NumericInput::Range);

    auto *delegate = new ExtendedDelegate();
    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);

    ui->entityScores_table->setItemDelegate(delegate);
    ui->entityScores_table->installEventFilter(new ViewEventFilter(this));

    connect(ui->entityScores_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::entityScores_onAdded);
}

void MCRPredCondition::initInvertedCondPage() {
    ui->inverted_condAreaInner->setLayout(&invertedCondLayout);
}

void MCRPredCondition::initNestedCondPage() {
    ui->nested_condAreaInner->setLayout(&nestedCondLayout);

    connect(ui->nested_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::nested_onAdded);
}

void MCRPredCondition::initRandChancePage() {
    connect(ui->randChance_slider, &QSlider::valueChanged, [ = ](int value) {
        ui->randChance_spinBox->setValue((double)value / 100);
    });
    connect(ui->randChance_spinBox,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            [ = ](double value) {
        ui->randChance_slider->setValue(qRound(value * 100));
    });
    connect(ui->randChance_lootingCheck,
            &QCheckBox::toggled, [ = ](bool checked) {
        ui->randChance_lootingLabel->setEnabled(checked);
        ui->randChance_lootingInput->setEnabled(checked);
    });
}

void MCRPredCondition::initTableBonusPage() {
    initComboModelView("enchantment", enchantmentsModel,
                       ui->tableBonus_enchantCombo);
    ui->toolEnchant_enchantCombo->setModel(&enchantmentsModel);

    ui->tableBonus_listView->setModel(&tableBonusModel);
    ui->tableBonus_listView->installEventFilter(&viewFilter);

    connect(ui->tableBonus_chanceSlider,
            &QSlider::valueChanged, [ = ](int value) {
        ui->tableBonus_chanceSpinBox->setValue((double)value / 100);
    });
    connect(ui->tableBonus_chanceSpinBox,
            qOverload<double>(&QDoubleSpinBox::valueChanged),
            [ = ](double value) {
        ui->tableBonus_chanceSlider->setValue(qRound(value * 100));
    });
    connect(ui->tableBonus_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::tableBonus_onAdded);
}

void MCRPredCondition::initToolEnchantPage() {
    ui->toolEnchant_levelsInput->setTypes(NumericInput::Range);

    auto *delegate = new ExtendedDelegate();
    delegate->setExNumInputTypes(NumericInput::Exact | NumericInput::Range);
    ui->toolEnchant_table->setItemDelegate(delegate);
    ui->toolEnchant_table->installEventFilter(&viewFilter);

    connect(ui->toolEnchant_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::toolEnchant_onAdded);
}

void MCRPredCondition::addInvertCondition(QJsonObject &json) const {
    json = QJsonObject({ { "condition", "minecraft:inverted" },
                           { "term", json } });
}

void MCRPredCondition::simplifyCondition(QVariantMap &condMap,
                                         int depth) const {
    const QString tab = QString(" ").repeated(depth);

    /*qDebug().noquote() << tab << "simplifyCondJson" << depth; */
    if (condMap.contains("condition")) {
        /*qDebug().noquote() << tab << "main condition:" << */
        condMap["condition"].toString();
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
                } else if (sub["condition"].toString().endsWith("alternative"))
                {
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
        } else if (condMap["condition"].toString().endsWith("alternative")) {
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
