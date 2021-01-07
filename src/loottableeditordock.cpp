#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"
#include "mcrpredcondition.h"
#include "loottableentry.h"
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

    ui->poolListView->setModel(&model);
    ui->conditionsContainer->setLayout(&conditionsLayout);
    ui->functionsContainer->setLayout(&functionsLayout);
    ui->entriesContainer->setLayout(&entriesLayout);

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::writeJson);
    connect(ui->readLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::readJson);


    connect(ui->addPoolButton, &QToolButton::clicked,
            this, &LootTableEditorDock::onAddPool);

    connect(ui->deletePoolButton, &QToolButton::clicked,
            this, &LootTableEditorDock::onDeletePool);
    connect(ui->poolListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this, &LootTableEditorDock::onCurrentPoolChanged);
    connect(ui->addCondButton, &QPushButton::clicked,
            this, &LootTableEditorDock::onAddCondition);
    connect(ui->addFunctBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::onAddFunction);
    connect(ui->addEntryButton, &QPushButton::clicked,
            this, &LootTableEditorDock::onAddEntry);

    checkPools();
    onAddPool();
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    onCurrentPoolChanged(QModelIndex(), ui->poolListView->currentIndex());

    QJsonObject root;

    root.insert("type", "minecraft:" +
                types[ui->lootTableTypeCombo->currentIndex()]);

    QJsonArray pools;
    for (int i = 0; i < model.rowCount(); ++i) {
        auto index    = model.index(i, 0);
        auto poolJson =
            model.data(index, Qt::UserRole + 1).toJsonObject();
        pools.push_back(poolJson);
    }
    root.insert("pools", pools);

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

    model.setRowCount(0);
    reset();

    QJsonArray pools = root.value("pools").toArray();
    for (auto poolRef : pools) {
        QStandardItem *item =
            new QStandardItem(QString("#%1").arg(model.rowCount() + 1));
        item->setData(poolRef.toObject());
        model.appendRow(item);
    }
    if (model.rowCount() > 0) {
        ui->poolListView->setCurrentIndex(model.index(0, 0));
    }
    checkPools();
}

void LootTableEditorDock::onAddPool() {
    /*qDebug() << "onAddPool"; */
    QStandardItem *item =
        new QStandardItem(QString("#%1").arg(model.rowCount() + 1));

    item->setData(writePoolJson());
    model.appendRow(item);

    auto index = model.indexFromItem(item);
    ui->poolListView->setCurrentIndex(index);
    checkPools();
}

void LootTableEditorDock::onDeletePool() {
    auto curIndex = ui->poolListView->currentIndex();

    /*qDebug() << "onDeletePool" << curIndex; */

    if (curIndex.isValid()) {
        /*ui->poolListView->clearSelection(); */
        model.removeRow(curIndex.row());

        for (int i = 0; i < model.rowCount(); ++i) {
            model.item(i)->setText(QString("#%1").arg(i + 1));
        }
        checkPools();
    }
}

void LootTableEditorDock::onCurrentPoolChanged(const QModelIndex &current,
                                               const QModelIndex &previous) {
    /*qDebug() << "onCurrentPoolChanged" << current << previous; */
    if (previous.isValid()) {
        auto moveOutJson = writePoolJson();
        model.itemFromIndex(previous)->setData(moveOutJson);
    }
    if (current.isValid() && (current != previous)) {
        auto moveInJson =
            model.itemFromIndex(current)->data().toJsonObject();
        readPoolJson(moveInJson);
    }
}

void LootTableEditorDock::checkPools() {
    auto curIndex = ui->poolListView->currentIndex();

    /*qDebug() << "checkPools" << curIndex; */

    ui->deletePoolButton->setEnabled(curIndex.isValid());
    ui->poolEditor->setEnabled(curIndex.isValid());
}

void LootTableEditorDock::reset() {
    /*qDebug() << "reset"; */
    ui->rollsInput->unset();
    ui->bonusRollsInput->unset();

    Glhp::deleteChildrenIn(ui->conditionsContainer);
    Glhp::deleteChildrenIn(ui->entriesContainer);
    Glhp::deleteChildrenIn(ui->functionsContainer);
}

void LootTableEditorDock::onAddCondition() {
    auto *cond = new MCRPredCondition(ui->conditionsContainer);

    cond->sizeHint().rheight() = cond->minimumHeight();
    conditionsLayout.addWidget(cond, 0);
}

void LootTableEditorDock::onAddFunction() {
    auto *funct = new LootTableFunction(ui->functionsContainer);

    funct->sizeHint().rheight() = funct->minimumHeight();
    functionsLayout.addWidget(funct, 0);
}

void LootTableEditorDock::onAddEntry() {
    auto *entry = new LootTableEntry(ui->entriesContainer);

    entry->setLootTableEditor(this);

    /*entry->sizeHint().rheight() = entry->minimumHeight(); */
    entriesLayout.addWidget(entry, 0);
}

QJsonObject LootTableEditorDock::writePoolJson() {
    QJsonObject root;

    /*qDebug() << "writePoolJson"; */

    if (!ui->bonusRollsInput->isCurrentlyUnset()
        && ui->bonusRollsInput->isVisible()) {
        root.insert("bonus_rolls", ui->bonusRollsInput->toJson());
    }
    root.insert("rolls", ui->rollsInput->toJson());

    auto conditions = Glhp::getJsonFromObjectsFromParent<MCRPredCondition>(
        ui->conditionsContainer);
    if (!conditions.isEmpty())
        root.insert("conditions", conditions);

    auto functions = Glhp::getJsonFromObjectsFromParent<LootTableFunction>(
        ui->functionsContainer);
    if (!functions.isEmpty())
        root.insert("functions", functions);

    auto entries = Glhp::getJsonFromObjectsFromParent<LootTableEntry>(
        ui->entriesContainer);
    if (!entries.isEmpty())
        root.insert("entries", entries);

    return root;
}

void LootTableEditorDock::readPoolJson(const QJsonObject &root) {
    /*qDebug() << "readPoolJson"; */

    ui->poolListView->setDisabled(true);

    reset();

    ui->rollsInput->fromJson(root.value("rolls"));
    if (root.contains("bonus_rolls"))
        ui->bonusRollsInput->fromJson(root.value("bonus_rolls"));

    if (root.contains("conditions")) {
        QJsonArray conditions = root.value("conditions").toArray();
        Glhp::loadJsonToObjectsToLayout<MCRPredCondition>(conditions,
                                                          conditionsLayout);
    }
    if (root.contains("functions")) {
        QJsonArray functions = root.value("functions").toArray();
        Glhp::loadJsonToObjectsToLayout<LootTableFunction>(functions,
                                                           functionsLayout);
    }
    if (root.contains("entries")) {
        QJsonArray entries = root.value("entries").toArray();
        Glhp::loadJsonToObjectsToLayout<LootTableEntry>(entries, entriesLayout);
    }

    ui->poolListView->setEnabled(true);
    ui->poolListView->setFocus();
}
