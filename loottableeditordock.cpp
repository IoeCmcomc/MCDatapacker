#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"
#include "mcrpredcondition.h"
#include "loottableentry.h"

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
    ui->entriesContainer->setLayout(&entriesLayout);

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this, &LootTableEditorDock::writeJson);
    connect(ui->addPoolButton, &QToolButton::clicked,
            this, &LootTableEditorDock::onAddPool);

    connect(ui->deletePoolButton, &QToolButton::clicked,
            this, &LootTableEditorDock::onDeletePool);
    connect(ui->poolListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this, &LootTableEditorDock::onCurrentPoolChanged);
    connect(ui->luckBasedCheck, &QCheckBox::toggled, [this](bool checked) {
        ui->bonusRollsLabel->setEnabled(checked);
        ui->bonusRollsInput->setEnabled(checked);
        emit isLuckBasedChanged(checked);
    });
    connect(ui->addCondButton, &QPushButton::clicked,
            this, &LootTableEditorDock::onAddCondition);
    connect(ui->addEntryButton, &QPushButton::clicked,
            this, &LootTableEditorDock::onAddEntry);

    checkPools();
    onAddPool();
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    ui->poolListView->clearSelection();
    auto currIndex = ui->poolListView->currentIndex();
    ui->poolListView->setCurrentIndex(QModelIndex());
    ui->poolListView->setCurrentIndex(currIndex);

    QJsonObject root;

    root.insert("type", "minecraft:" +
                types[ui->lootTableTypeCombo->currentIndex()]);

    QJsonArray pools;
    for (int i = 0; i < model.rowCount(); ++i) {
        auto index    = model.index(i, 0);
        auto poolJson =
            model.data(index, Qt::UserRole + 1).value<QJsonObject>();
        pools.push_back(poolJson);
    }
    root.insert("pools", pools);

    qobject_cast<MainWindow*>(parent())->
    setCodeEditorText(QJsonDocument(root).toJson());
}

void LootTableEditorDock::onAddPool() {
    qDebug() << "onAddPool";
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
    if (current.isValid()) {
        auto moveInJson =
            model.itemFromIndex(current)->data().value<QJsonObject>();
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
    ui->luckBasedCheck->setChecked(false);
    ui->rollsInput->unset();
    ui->bonusRollsInput->unset();
}

void LootTableEditorDock::onAddCondition() {
    MCRPredCondition *cond = new MCRPredCondition(ui->conditionsContainer);

    cond->sizeHint().rheight() = cond->minimumHeight();
    conditionsLayout.addWidget(cond, 0);
}

void LootTableEditorDock::onAddEntry() {
    LootTableEntry *entry = new LootTableEntry(ui->entriesContainer);

    entry->setLootTableEditor(this);

    /*entry->sizeHint().rheight() = entry->minimumHeight(); */
    entriesLayout.addWidget(entry, 0);
}

QJsonObject LootTableEditorDock::writePoolJson() {
    QJsonObject root;

    if (!ui->bonusRollsInput->isCurrentlyUnset()
        && ui->bonusRollsInput->isVisible()) {
        root.insert("bonus_rolls", ui->bonusRollsInput->toJson());
    }
    root.insert("rolls", ui->rollsInput->toJson());

    int childCount = ui->entriesContainer->children().count();
    if (childCount != 0) {
        QJsonArray entries;
        for (auto *child : ui->entriesContainer->children()) {
            LootTableEntry *childEntry = qobject_cast<LootTableEntry*>(child);
            if (childEntry != nullptr)
                entries.push_back(childEntry->toJson());
        }
        root.insert("children", entries);
    }

    return root;
}

void LootTableEditorDock::readPoolJson(const QJsonObject &root) {
    reset();
    ui->rollsInput->fromJson(root.value("rolls"));
    ui->luckBasedCheck->setChecked(root.contains("bonus_rolls"));
    if (root.contains("bonus_rolls"))
        ui->bonusRollsInput->fromJson(root.value("bonus_rolls"));
}
