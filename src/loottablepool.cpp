#include "loottablepool.h"
#include "ui_loottablepool.h"

#include "mcrpredcondition.h"
#include "loottableentry.h"
#include "loottablefunction.h"

#include <QJsonObject>

LootTablePool::LootTablePool(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTablePool) {
    ui->setupUi(this);

    auto *cond = new MCRPredCondition();
    ui->conditionsInterface->setMainWidget(cond);

    ui->conditionsInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&MCRPredCondition::fromJson));
    ui->conditionsInterface->mapToGetter(&MCRPredCondition::toJson, cond);

    auto *func = new LootTableFunction();
    ui->functionsInterface->setupMainWidget(func);

    auto *entry = new LootTableEntry();
    ui->entriesInterface->setupMainWidget(entry);
}

LootTablePool::~LootTablePool() {
    delete ui;
}

void LootTablePool::changeEvent(QEvent *event) {
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);
}

QJsonObject LootTablePool::toJson() const {
    QJsonObject root;

    if (!ui->bonusRollsInput->isCurrentlyUnset()
        && ui->bonusRollsInput->isVisible()) {
        root.insert("bonus_rolls", ui->bonusRollsInput->toJson());
    }
    root.insert("rolls", ui->rollsInput->toJson());

    auto &&conditions = ui->conditionsInterface->json();
    if (!conditions.isEmpty())
        root.insert("conditions", conditions);

    auto &&functions = ui->functionsInterface->json();
    if (!functions.isEmpty())
        root.insert("functions", functions);

    auto &&entries = ui->entriesInterface->json();
    if (!entries.isEmpty())
        root.insert("entries", entries);

    return root;
}

void LootTablePool::fromJson(QJsonObject root) {
    reset();

    ui->rollsInput->fromJson(root.value("rolls"));
    if (root.contains("bonus_rolls"))
        ui->bonusRollsInput->fromJson(root.value("bonus_rolls"));

    if (root.contains("conditions")) {
        ui->conditionsInterface->setJson(root.value("conditions").toArray());
    }
    if (root.contains("functions")) {
        ui->functionsInterface->setJson(root.value("functions").toArray());
    }
    if (root.contains("entries")) {
        ui->entriesInterface->setJson(root.value("entries").toArray());
    }
}

void LootTablePool::reset() {
    ui->rollsInput->unset();
    ui->bonusRollsInput->unset();

    ui->entriesInterface->setJson({});
    ui->functionsInterface->setJson({});
    ui->conditionsInterface->setJson({});
}
