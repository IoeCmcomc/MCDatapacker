#include "mcrpredcondition.h"
#include "ui_mcrpredcondition.h"

#include "mainwindow.h"
#include "mcrinvitem.h"
#include "extendeddelegate.h"
#include "itemconditiondialog.h"
#include "locationconditiondialog.h"
#include "entityconditiondialog.h"

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
    initTableBonusPage();
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

void MCRPredCondition::onTypeChanged(const int &i) {
    ui->stackedWidget->setCurrentIndex(i);

    if (ui->stackedWidget->currentWidget() == ui->inverted) {
        if (invertedCondLayout.isEmpty()) {
            MCRPredCondition *cond = new MCRPredCondition(
                ui->nested_condAreaInner);

            cond->setMinimumHeight(300);
            invertedCondLayout.addWidget(cond, 0);
        }
    }
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
    enchantItem->setData(ui->toolEnchant_enchantCombo->currentData());
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
    /*ui->nested_condArea->setLayout(&nestedCondLayout); */

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
