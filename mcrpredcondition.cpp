#include "mcrpredcondition.h"
#include "ui_mcrpredcondition.h"

#include "mainwindow.h"
#include "mcrinvitem.h"
#include "extendednumericdelegate.h"

#include <QContextMenuEvent>
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
    initEntityScoresPage();
    initInvertedCondPage();
    initNestedCondPage();
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

bool MCRPredCondition::eventFilter(QObject *obj, QEvent *event) {
    /*qDebug() << event->type() << event; */
    if (event->type() == QEvent::ContextMenu) {
        /*auto *abstractView = qobject_cast<QTableView*>(obj); */
        auto *abstractView = qobject_cast<QAbstractItemView*>(obj);
        if (abstractView != nullptr) {
            auto *model     = abstractView->model();
            auto *menuEvent = static_cast<QContextMenuEvent*>(event);
            auto  finalPos  = abstractView->viewport()->
                              mapFromGlobal(abstractView->
                                            mapToGlobal(menuEvent->pos()));
            auto index = abstractView->indexAt(finalPos);
            if (index.isValid()) {
                model->removeRow(index.row());
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void MCRPredCondition::onTypeChanged(const int &i) {
    ui->stackedWidget->setCurrentIndex(i);

    if (ui->stackedWidget->currentWidget() == ui->inverted) {
        if (invertedCondLayout.isEmpty()) {
            MCRPredCondition *cond = new MCRPredCondition(
                ui->nested_condAreaInner);

            cond->setMinimumHeight(470);
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
    stateItem->setToolTip(deleteableToolTip);
    QStandardItem *valueItem = new QStandardItem(
        ui->blockState_valueEdit->text());
    valueItem->setToolTip(deleteableToolTip);

    blockStateModel.appendRow({ stateItem, valueItem });
}

void MCRPredCondition::entityScores_onAdded() {
    if (ui->entityScores_objectiveEdit->text().isEmpty())
        return;

    QStandardItem *objItem = new QStandardItem(
        ui->entityScores_objectiveEdit->text());
    objItem->setToolTip(deleteableToolTip);
    QStandardItem *valueItem = new QStandardItem();
    auto           json      = ui->entityScores_valueInput->toJson();
    valueItem->setData(json, Qt::DisplayRole);
    valueItem->setToolTip(deleteableToolTip);
    entityScoresModel.appendRow({ objItem, valueItem });
}

void MCRPredCondition::nested_onAdded() {
    MCRPredCondition *cond = new MCRPredCondition(ui->nested_condAreaInner);

    cond->setMinimumHeight(470);
    nestedCondLayout.addWidget(cond, 0);
}

void MCRPredCondition::tableBonus_onAdded() {
    QStandardItem *chanceItem = new QStandardItem();

    chanceItem->setData(ui->tableBonus_chanceSpinBox->value(),
                        Qt::DisplayRole);
    chanceItem->setToolTip(deleteableToolTip);
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
    enchantItem->setToolTip(deleteableToolTip);
    QStandardItem *levelsItem = new QStandardItem();
    auto           json       = ui->toolEnchant_levelsInput->toJson();
    levelsItem->setData(json, Qt::DisplayRole);
    levelsItem->setToolTip(deleteableToolTip);
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

    blockStateModel.setHorizontalHeaderItem(0, stateItem);
    blockStateModel.setHorizontalHeaderItem(1, valuesItem);

    ui->blockState_listView->setModel(&blockStateModel);
    ui->blockState_listView->installEventFilter(this);


    connect(ui->blockState_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::blockStates_onAdded);
}

void MCRPredCondition::initEntityScoresPage() {
    ui->entityScores_valueInput->setTypes(
        ExtendedNumericInput::Exact | ExtendedNumericInput::Range);

    QStandardItem *objItem = new QStandardItem("Objective");
    objItem->setToolTip("An score objective of the entity.");
    QStandardItem *valuesItem = new QStandardItem("Score");
    valuesItem->setToolTip("A value of the objective.");

    entityScoresModel.setHorizontalHeaderItem(0, objItem);
    entityScoresModel.setHorizontalHeaderItem(1, valuesItem);

    ui->entityScores_listView->setModel(&entityScoresModel);
    ui->entityScores_listView->installEventFilter(this);
    auto *delegate = new ExtendedNumericDelegate();
    delegate->setExNumInputTypes(ExtendedNumericInput::Exact
                                 | ExtendedNumericInput::Range);
    ui->entityScores_listView->setItemDelegate(delegate);

    connect(ui->entityScores_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::entityScores_onAdded);
}

void MCRPredCondition::initInvertedCondPage() {
    ui->inverted_condAreaInner->setLayout(&nestedCondLayout);
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
            &QCheckBox::stateChanged, [ = ](bool checked) {
        ui->randChance_lootingLabel->setEnabled(checked);
        ui->randChance_lootingInput->setEnabled(checked);
    });
}

void MCRPredCondition::initTableBonusPage() {
    auto enchantmentsInfo = MainWindow::readMCRInfo("enchantments");

    for (auto key : enchantmentsInfo.keys()) {
        QStandardItem *item = new QStandardItem();
        auto           map  = enchantmentsInfo.value(key).toMap();
        item->setText(map.value("name").toString());
        item->setData(key);
        item->setToolTip(map.value("summary").toString());

        enchantmentsModel.appendRow(item);
    }
    ui->tableBonus_enchantCombo->setModel(&enchantmentsModel);
    ui->toolEnchant_enchantCombo->setModel(&enchantmentsModel);

    ui->tableBonus_listView->setModel(&tableBonusModel);
    ui->tableBonus_listView->installEventFilter(this);

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
    ui->toolEnchant_levelsInput->setTypes(ExtendedNumericInput::Range);

    QStandardItem *enchantItem = new QStandardItem("Enchantment");
    enchantItem->setToolTip("An enchantment of the tool.");
    QStandardItem *levelsItem = new QStandardItem("Levels");
    levelsItem->setToolTip("The minimun and maximun levels of the enchantment.");

    toolEnchantModel.setHorizontalHeaderItem(0, enchantItem);
    toolEnchantModel.setHorizontalHeaderItem(1, levelsItem);

    ui->toolEnchant_listView->setModel(&toolEnchantModel);
    ui->toolEnchant_listView->installEventFilter(this);
    auto *delegate = new ExtendedNumericDelegate();
    delegate->setExNumInputTypes(ExtendedNumericInput::Range);
    ui->toolEnchant_listView->setItemDelegate(delegate);

    connect(ui->toolEnchant_addBtn, &QPushButton::clicked,
            this, &MCRPredCondition::toolEnchant_onAdded);
}
