#include "loottable.h"
#include "ui_loottable.h"

#include "loottablepool.h"
#include "loottablefunction.h"

#include "modelfunctions.h"
#include "globalhelpers.h"
#include "game.h"
#include "platforms/windows_specific.h"


LootTable::LootTable(QWidget *parent)
    : QTabWidget(parent), ui(new Ui::LootTable) {
    ui->setupUi(this);

    if (Game::version() < Game::v1_16) {
        hideComboRow(ui->lootTableTypeCombo, 8);
        hideComboRow(ui->lootTableTypeCombo, 9);
        hideComboRow(ui->lootTableTypeCombo, 10);
        hideComboRow(ui->lootTableTypeCombo, 11);
    }
    if (Game::version() < Game::v1_20) {
        ui->randomSeqEdit->hide();
        ui->randomSeqLabel->hide();
        hideComboRow(ui->lootTableTypeCombo, 12);
        hideComboRow(ui->lootTableTypeCombo, 13);
    }

    connect(ui->poolsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTable::updatePoolsTab);
    connect(ui->functionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTable::updateFunctionsTab);

    auto *pool = new LootTablePool();
    ui->poolsInterface->setupMainWidget(pool);

    auto *func = new LootTableFunction();
    ui->functionsInterface->setupMainWidget(func);
}

LootTable::~LootTable() {
    delete ui;
}

void LootTable::fromJson(QJsonObject &&root) {
    if (root.isEmpty() || !root.contains("pools")) {
        return;
    }

    QString type = root.value("type").toString();
    Glhp::removePrefix(type);

    const int index = types.indexOf(type);
    if (index > -1) {
        const auto &&model =
            static_cast<QStandardItemModel *>(ui->lootTableTypeCombo->model());
        const auto &&item = model->item(index, 0);
        if (!item->isEnabled())
            return;

        ui->lootTableTypeCombo->setCurrentIndex(index);
    }

    if (Game::version() >= Game::v1_20) {
        ui->randomSeqEdit->setText(root.value("random_sequence").toString());
    }

    ui->poolsInterface->setJson(root.value("pools").toArray());
    ui->functionsInterface->setJson(root.value("functions").toArray());
}

QJsonObject LootTable::toJson() const {
    QJsonObject root;

    if (ui->lootTableTypeCombo->currentIndex() == 0) {
        return {};
    }
    const QString &&type = QStringLiteral("minecraft:") +
                           types[ui->lootTableTypeCombo->currentIndex()];

    root.insert("type", type);

    if (Game::version() >= Game::v1_20) {
        if (auto &&sequence = ui->randomSeqEdit->text(); !sequence.isEmpty()) {
            root["random_sequence"] = sequence;
        }
    }

    QJsonArray pools = ui->poolsInterface->json();
    if (!pools.isEmpty())
        root.insert("pools", pools);

    QJsonArray functions = ui->functionsInterface->json();
    if (!functions.isEmpty())
        root.insert("functions", functions);

    return root;
}

void LootTable::changeEvent(QEvent *e) {
    QTabWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updatePoolsTab(ui->poolsInterface->entriesCount());
        updateFunctionsTab(ui->functionsInterface->entriesCount());
    }
}

void LootTable::updatePoolsTab(int size) {
    setTabText(1, tr("Pools (%1)").arg(size));
}

void LootTable::updateFunctionsTab(int size) {
    setTabText(2, tr("Functions (%1)").arg(size));
}
