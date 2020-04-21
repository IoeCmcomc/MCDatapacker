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

    connect(ui->deleteButton, &QPushButton::clicked, [this]() {
        deleteLater();
    });
    connect(ui->conditionTypeCombo,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this, &MCRPredCondition::onTypeChanged);
    MainWindow *mainWin;
    for (auto wid : qApp->topLevelWidgets()) {
        if (wid->objectName() == "MainWindow") {
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

    root.insert("condition",
                "minecraft:" +
                condTypes[ui->conditionTypeCombo->currentIndex()]);

    switch (ui->conditionTypeCombo->currentIndex()) {
    case 0: { /*Block states */
        if (ui->blockState_blockCombo->currentIndex() != 0) {
            auto invItem = ui->blockState_blockCombo->currentData(
                Qt::UserRole + 1).value<MCRInvItem>();
            root.insert("block", invItem.getNamespacedID());
        }
        QJsonObject states = LocationConditionDialog::jsonFromTable(
            blockStatesModel);
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
        for (auto row = 0; row < entityScoresModel.rowCount(); ++row) {
            auto objective = entityScoresModel.item(row, 0)->text();
            auto value     = entityScoresModel.item(row, 1)->
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
        for (int i = 0; i < toolEnchantModel.rowCount(); ++i) {
            auto id =
                toolEnchantModel.item(i, 0)->data(Qt::UserRole + 1).toString();
            auto levels = toolEnchantModel.item(i, 1)->
                          data(Qt::DisplayRole).toJsonValue();
            enchantments.push_back(QJsonObject({ { QStringLiteral("enchantment"),
                                                   id },
                                                   { QStringLiteral("levels"),
                                                     levels } }));
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

void MCRPredCondition::fromJson(const QJsonObject &value) {
}

void MCRPredCondition::onTypeChanged(const int &i) {
    ui->stackedWidget->setCurrentIndex(i);

    if ((ui->stackedWidget->currentWidget() == ui->inverted) &&
        invertedCondLayout.isEmpty()) {
        if (depth < 10) {
            MCRPredCondition *cond = new MCRPredCondition(
                ui->nested_condAreaInner);
            cond->setDepth(depth + 1);
            cond->setMinimumHeight(350);
            invertedCondLayout.addWidget(cond, 0);
        } else {
            ui->inverted_condAreaInner->setText(
                "Inverting an inverted condition is uesless.");
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

void MCRPredCondition::setDepth(int value) {
    depth = value;
}

void MCRPredCondition::blockStates_onAdded() {
    if (ui->blockState_stateEdit->text().isEmpty()
        || ui->blockState_valueEdit->text().isEmpty())
        return;

    QStandardItem *stateItem = new QStandardItem(
        ui->blockState_stateEdit->text());
    stateItem->setToolTip(deletiveToolTip);
    QStandardItem *valueItem = new QStandardItem(
        ui->blockState_valueEdit->text());
    valueItem->setToolTip(deletiveToolTip);

    blockStatesModel.appendRow({ stateItem, valueItem });
}

void MCRPredCondition::entityScores_onAdded() {
    if (ui->entityScores_objectiveEdit->text().isEmpty())
        return;

    QStandardItem *objItem = new QStandardItem(
        ui->entityScores_objectiveEdit->text());
    objItem->setToolTip(deletiveToolTip);
    QStandardItem *valueItem = new QStandardItem();
    auto           json      = ui->entityScores_valueInput->toJson();
    valueItem->setData(json, Qt::DisplayRole);
    valueItem->setToolTip(deletiveToolTip);
    entityScoresModel.appendRow({ objItem, valueItem });
}

void MCRPredCondition::nested_onAdded() {
    MCRPredCondition *cond = new MCRPredCondition(ui->nested_condAreaInner);

    cond->setMinimumHeight(300);
    nestedCondLayout.addWidget(cond, 0);
}

void MCRPredCondition::setupRefCombo() {
    /*qDebug() << "setupRefCombo"; */
    if (condRefsModel.rowCount() > 0)
        condRefsModel.clear();
    auto predRefIDs = GlobalHelpers::fileIDList(MainWindow::getCurDir(),
                                                "predicates");

    for (auto predRef : predRefIDs)
        condRefsModel.appendRow(new QStandardItem(predRef));
    ui->ref_nameCombo->setModel(&condRefsModel);
}

void MCRPredCondition::tableBonus_onAdded() {
    QStandardItem *chanceItem = new QStandardItem();

    chanceItem->setData(ui->tableBonus_chanceSpinBox->value(),
                        Qt::DisplayRole);
    chanceItem->setToolTip(deletiveToolTip);
    tableBonusModel.appendRow(chanceItem);
}

void MCRPredCondition::toolEnchant_onAdded() {
    if (ui->toolEnchant_levelsInput->getMinimum() == 0
        || ui->toolEnchant_levelsInput->getMaximum() == 0) {
        return;
    }
    QString enchantmentText = ui->toolEnchant_enchantCombo->currentText();
    if (!toolEnchantModel.findItems(enchantmentText).isEmpty())
        return;

    QStandardItem *enchantItem = new QStandardItem(enchantmentText);
    enchantItem->setData(ui->toolEnchant_enchantCombo->currentData(Qt::UserRole
                                                                   + 1));
    enchantItem->setEditable(false);
    enchantItem->setToolTip(deletiveToolTip);
    QStandardItem *levelsItem = new QStandardItem();
    auto           json       = ui->toolEnchant_levelsInput->toJson();
    levelsItem->setData(json, Qt::DisplayRole);
    levelsItem->setToolTip(deletiveToolTip);
    toolEnchantModel.appendRow({ enchantItem, levelsItem });
}

void MCRPredCondition::initBlockStatesPage() {
    auto blocksInfo = MainWindow::getMCRInfo("block");

    blocksModel.appendRow(new QStandardItem(tr("(not selected)")));
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

    QStandardItem *stateItem = new QStandardItem("State");
    stateItem->setToolTip("An state of the block.");
    QStandardItem *valuesItem = new QStandardItem("Value");
    valuesItem->setToolTip("A value of the state.");

    initModelView(blockStatesModel, ui->blockState_listView,
                  { stateItem, valuesItem });

    connect(ui->blockState_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::blockStates_onAdded);
}

void MCRPredCondition::initEntityScoresPage() {
    ui->entityScores_valueInput->setTypes(
        NumericInput::Exact | NumericInput::Range);

    QStandardItem *objItem = new QStandardItem("Objective");
    objItem->setToolTip("An score objective of the entity.");
    QStandardItem *valuesItem = new QStandardItem("Score");
    valuesItem->setToolTip("A value of the objective.");

    auto *delegate = new ExtendedDelegate();
    delegate->setExNumInputTypes(NumericInput::Exact
                                 | NumericInput::Range);

    initModelView(entityScoresModel, ui->entityScores_listView,
                  { objItem, valuesItem }, delegate);

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

    QStandardItem *enchantItem = new QStandardItem("Enchantment");
    enchantItem->setToolTip("An enchantment of the tool.");
    QStandardItem *levelsItem = new QStandardItem("Levels");
    levelsItem->setToolTip("The minimun and maximun levels of the enchantment.");

    auto *delegate = new ExtendedDelegate();
    delegate->setExNumInputTypes(NumericInput::Range);

    initModelView(toolEnchantModel, ui->toolEnchant_listView,
                  { enchantItem, levelsItem }, delegate);

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
/*
                      condMap.clear();
                      condMap.unite(subTerm);
 */
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
