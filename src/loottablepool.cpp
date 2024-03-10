#include "loottablepool.h"
#include "ui_loottablepool.h"

#include "loottablecondition.h"
#include "loottableentry.h"
#include "loottablefunction.h"

#include <QJsonObject>


LootTablePool::LootTablePool(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTablePool) {
    ui->setupUi(this);
}

LootTablePool::~LootTablePool() {
    delete ui;
}

void LootTablePool::init() {
    auto *cond = new LootTableCondition();

    ui->conditionsInterface->setMainWidget(cond);

    ui->conditionsInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&LootTableCondition::fromJson));
    ui->conditionsInterface->mapToGetter(&LootTableCondition::toJson, cond);

    auto *func = new LootTableFunction();
    ui->functionsInterface->setupMainWidget(func);

    auto *entry = new LootTableEntry();
    ui->entriesInterface->setupMainWidget(entry);

    connect(ui->entriesInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTablePool::updateEntriesTab);
    connect(ui->functionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTablePool::updateFunctionsTab);
    connect(ui->conditionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTablePool::updateConditionsTab);

    m_controller.addMapping(QStringLiteral("rolls"), ui->rollsInput, true);
    m_controller.addMapping(QStringLiteral("bonus_rolls"), ui->bonusRollsInput);
    m_controller.addMapping(QStringLiteral("entries"), ui->entriesInterface);
    m_controller.addMapping(QStringLiteral("functions"),
                            ui->functionsInterface);
    m_controller.addMapping(QStringLiteral("conditions"),
                            ui->conditionsInterface);

    updateCounts();
}

void LootTablePool::changeEvent(QEvent *event) {
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateCounts();
    }
}

void LootTablePool::showEvent(QShowEvent *event) {
    std::call_once(m_fullyInitialized, &LootTablePool::init, this);
    QTabWidget::showEvent(event);
}

QJsonObject LootTablePool::toJson() const {
    QJsonObject root;

    m_controller.putValueTo(root, {});
    return root;
}

void LootTablePool::fromJson(const QJsonObject &root) {
    std::call_once(m_fullyInitialized, &LootTablePool::init, this);
    reset();
    m_controller.setValueFrom(root, {});
}

void LootTablePool::reset() {
    ui->rollsInput->unset();
    ui->bonusRollsInput->unset();

    ui->entriesInterface->setJson({});
    ui->functionsInterface->setJson({});
    ui->conditionsInterface->setJson({});
}

void LootTablePool::updateCounts() {
    updateEntriesTab(ui->entriesInterface->entriesCount());
    updateFunctionsTab(ui->functionsInterface->entriesCount());
    updateConditionsTab(ui->conditionsInterface->entriesCount());
}

void LootTablePool::updateEntriesTab(int size) {
    setTabText(1, tr("Entries (%1)").arg(size));
}

void LootTablePool::updateFunctionsTab(int size) {
    setTabText(2, tr("Functions (%1)").arg(size));
}

void LootTablePool::updateConditionsTab(int size) {
    setTabText(3, tr("Conditions (%1)").arg(size));
}
