#include "loottableeditordock.h"
#include "ui_loottableeditordock.h"

#include "mainwindow.h"

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

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this,
            &LootTableEditorDock::writeJson);
    connect(ui->addPoolButton,
            &QToolButton::clicked,
            this, &LootTableEditorDock::onAddPool);
    connect(ui->deletePoolButton,
            &QToolButton::clicked,
            this, &LootTableEditorDock::onDeletePool);
    connect(ui->poolListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &LootTableEditorDock::onPoolSelectionChanged);
    connect(ui->bonusRollsCheck, &QCheckBox::toggled, [this](bool checked) {
        ui->bonusRollsInput->setEnabled(checked);
    });

    checkPools();
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    onPoolSelectionChanged();

    QJsonObject   root;
    const QString types[] = {
        QStringLiteral("empty"),              QStringLiteral("entity"),
        QStringLiteral("block"),              QStringLiteral("chest"),
        QStringLiteral("fishing"),            QStringLiteral("gift"),
        QStringLiteral("advancement_reward"),
        QStringLiteral("generic")
    };

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
    QStandardItem *item =
        new QStandardItem(QString("#%1").arg(model.rowCount() + 1));

    onPoolSelectionChanged();
    item->setData(writePoolJson());
    model.appendRow(item);

    auto index = model.indexFromItem(item);
    ui->poolListView->selectionModel()->
    setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    ui->poolListView->selectionModel()->
    select(index, QItemSelectionModel::ClearAndSelect);
}

void LootTableEditorDock::onDeletePool() {
    auto indexes = ui->poolListView->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        auto index = indexes[0];
        poolDeleted       = true;
        indexBeforeDelete = index;
        model.removeRow(index.row());

        onPoolSelectionChanged();

        for (int i = 0; i < model.rowCount(); ++i) {
            model.item(i)->setText(QString("#%1").arg(i + 1));
        }
    }
}

void LootTableEditorDock::onPoolSelectionChanged() {
    /*qDebug() << "onPoolSelectionChanged"; */
    checkPools();
    auto indexes = ui->poolListView->selectionModel()->selectedIndexes();
    /*qDebug() << indexes; */
    QModelIndex index;
    if (!indexes.isEmpty()) {
        if (curPoolIndex.isValid()) {
            auto moveOutJson = writePoolJson();
            model.itemFromIndex(curPoolIndex)->setData(moveOutJson);
            /*qDebug() << "move out:" << moveOutJson; */
        }
        index = indexes[0];
    }
    /*qDebug() << poolDeleted << index.row() << indexBeforeDelete.row() << */
    model.rowCount();
    if (poolDeleted && (index.row() < model.rowCount()) &&
        (index.row() > indexBeforeDelete.row())) {
        /*index       = index.siblingAtRow(index.row() - 1); */
        poolDeleted = false;
    }

    curPoolIndex = QModelIndex();
    if (index.isValid()) {
        auto moveInJson =
            model.itemFromIndex(index)->data().value<QJsonObject>();
        readPoolJson(moveInJson);
        /*qDebug() << "In:" << moveInJson; */
        curPoolIndex = index;
    }

    /*qDebug() << "curIndex" << index.row() << curPoolIndex.row(); */
}

void LootTableEditorDock::checkPools() {
    auto indexes = ui->poolListView->selectionModel()->selectedIndexes();

    ui->deletePoolButton->setDisabled(indexes.isEmpty());
    ui->poolEditor->setDisabled(indexes.isEmpty());
}

QJsonObject LootTableEditorDock::writePoolJson() {
    QJsonObject root;

    if (ui->bonusRollsCheck->isChecked())
        root.insert("bonus_rolls", ui->bonusRollsInput->toJson());
    root.insert("rolls", ui->rollsInput->toJson());
    return root;
}

void LootTableEditorDock::readPoolJson(const QJsonObject &root) {
    ui->rollsInput->fromJson(root.value("rolls"));
    ui->bonusRollsCheck->setChecked(root.contains("bonus_rolls"));
    if (root.contains("bonus_rolls"))
        ui->bonusRollsInput->fromJson(root.value("bonus_rolls"));
}
