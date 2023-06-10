#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"
#include "loottablepool.h"
#include "loottablefunction.h"

#include "globalhelpers.h"
#include "game.h"
#include "platforms/windows_specific.h"

#include <QDebug>
#include <QJsonArray>
#include <QAction>
#include <QItemSelection>
#include <QJsonDocument>
#include <QListView>

LootTableEditorDock::LootTableEditorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LootTableEditorDock) {
    ui->setupUi(this);

    if (Game::version() < Game::v1_16) {
        if (auto *view =
                qobject_cast<QListView *>(ui->lootTableTypeCombo->view())) {
            const auto &&model =
                static_cast<QStandardItemModel *>(ui->lootTableTypeCombo->model());
            for (int i = 8; i < view->model()->rowCount(); ++i) {
                view->setRowHidden(i, true);
                model->item(i, 0)->setEnabled(false);
            }
        }
    }
    if (Game::version() < Game::v1_20) {
        ui->randomSeqEdit->hide();
        ui->randomSeqLabel->hide();
    }

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::writeJson);
    connect(ui->readLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::readJson);
    connect(ui->poolsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableEditorDock::updatePoolsTab);
    connect(ui->functionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableEditorDock::updateFunctionsTab);

    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
        if (floating) {
            Windows::setDarkFrameIfDarkMode(this);
            resize(minimumWidth(), height());
        }
    });

    auto *pool = new LootTablePool();
    ui->poolsInterface->setupMainWidget(pool);

    auto *func = new LootTableFunction();
    ui->functionsInterface->setupMainWidget(func);
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    QJsonObject root;

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

    qobject_cast<MainWindow *>(parent())->
    setCodeEditorText(QJsonDocument(root).toJson());
}

void LootTableEditorDock::readJson() {
    QString input =
        qobject_cast<MainWindow *>(parent())->getCodeEditorText();
    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    QJsonObject root = json_doc.object();
    if (root.isEmpty() || !root.contains("pools")) {
        return;
    }

    QString type = root.value("type").toString();
    Glhp::removePrefix(type, "minecraft:"_QL1);

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

void LootTableEditorDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updatePoolsTab(ui->poolsInterface->entriesCount());
        updateFunctionsTab(ui->functionsInterface->entriesCount());
    }
}

void LootTableEditorDock::updatePoolsTab(int size) {
    ui->tabWidget->setTabText(0, tr("Pools (%1)").arg(size));
}

void LootTableEditorDock::updateFunctionsTab(int size) {
    ui->tabWidget->setTabText(1, tr("Functions (%1)").arg(size));
}
