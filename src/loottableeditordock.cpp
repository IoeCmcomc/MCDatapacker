#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"
#include "loottablepool.h"
#include "loottablefunction.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QJsonArray>
#include <QAction>
#include <QItemSelection>
#include <QJsonDocument>

LootTableEditorDock::LootTableEditorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LootTableEditorDock) {
    ui->setupUi(this);

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::writeJson);
    connect(ui->readLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::readJson);

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

    root.insert("type", "minecraft:" +
                types[ui->lootTableTypeCombo->currentIndex()]);

    QJsonArray pools = ui->poolsInterface->json();
    if (!pools.isEmpty())
        root.insert("pools", pools);

    QJsonArray functions = ui->functionsInterface->json();
    if (!functions.isEmpty())
        root.insert("functions", functions);

    qobject_cast<MainWindow*>(parent())->
    setCodeEditorText(QJsonDocument(root).toJson());
}

void LootTableEditorDock::readJson() {
    QString input =
        qobject_cast<MainWindow*>(parent())->getCodeEditorText();
    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    QJsonObject root = json_doc.object();
    if (root.isEmpty() || !root.contains("pools")) {
        return;
    }

    QString type = root.value("type").toString();
    Glhp::removePrefix(type, "minecraft:");

    if (types.indexOf(type) > -1)
        ui->lootTableTypeCombo->setCurrentIndex(types.indexOf(type));

    ui->poolsInterface->setJson(root.value("pools").toArray());
    ui->functionsInterface->setJson(root.value("functions").toArray());
}

void LootTableEditorDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);
}
