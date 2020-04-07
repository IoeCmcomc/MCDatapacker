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

    ui->pollListView->setModel(&model);

    connect(ui->writeLootTableBtn, &QPushButton::clicked,
            this,
            &LootTableEditorDock::writeJson);
    connect(ui->addPollButton,
            &QToolButton::clicked,
            this, &LootTableEditorDock::onAddPoll);
    connect(ui->deletePoolButton,
            &QToolButton::clicked,
            this, &LootTableEditorDock::onDeletePoll);
    connect(ui->pollListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &LootTableEditorDock::onPollSelectionChanged);
    connect(ui->bonusRollsCheck, &QCheckBox::toggled, [this](bool checked) {
        ui->bonusRollsInput->setEnabled(checked);
    });

    checkPolls();
}

LootTableEditorDock::~LootTableEditorDock() {
    delete ui;
}

void LootTableEditorDock::writeJson() {
    onPollSelectionChanged();

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

    QJsonArray polls;
    for (int i = 0; i < model.rowCount(); ++i) {
        auto index    = model.index(i, 0);
        auto pollJson =
            model.data(index, Qt::UserRole + 1).value<QJsonObject>();
        polls.push_back(pollJson);
    }
    root.insert("polls", polls);

    qobject_cast<MainWindow*>(parent())->
    setCodeEditorText(QJsonDocument(root).toJson());
}

void LootTableEditorDock::onAddPoll() {
    QStandardItem *item =
        new QStandardItem(QString("#%1").arg(model.rowCount() + 1));

    onPollSelectionChanged();
    item->setData(writePollJson());
    model.appendRow(item);

    auto index = model.indexFromItem(item);
    ui->pollListView->selectionModel()->
    setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    ui->pollListView->selectionModel()->
    select(index, QItemSelectionModel::ClearAndSelect);
}

void LootTableEditorDock::onDeletePoll() {
    auto indexes = ui->pollListView->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        auto index = indexes[0];
        pollDeleted       = true;
        indexBeforeDelete = index;
        model.removeRow(index.row());

        onPollSelectionChanged();

        for (int i = 0; i < model.rowCount(); ++i) {
            model.item(i)->setText(QString("#%1").arg(i + 1));
        }
    }
}

void LootTableEditorDock::onPollSelectionChanged() {
    /*qDebug() << "onPollSelectionChanged"; */
    checkPolls();
    auto indexes = ui->pollListView->selectionModel()->selectedIndexes();
    /*qDebug() << indexes; */
    QModelIndex index;
    if (!indexes.isEmpty()) {
        if (curPollIndex.isValid()) {
            auto moveOutJson = writePollJson();
            model.itemFromIndex(curPollIndex)->setData(moveOutJson);
            /*qDebug() << "move out:" << moveOutJson; */
        }
        index = indexes[0];
    }
    /*qDebug() << pollDeleted << index.row() << indexBeforeDelete.row() << */
    model.rowCount();
    if (pollDeleted && (index.row() < model.rowCount()) &&
        (index.row() > indexBeforeDelete.row())) {
        /*index       = index.siblingAtRow(index.row() - 1); */
        pollDeleted = false;
    }

    curPollIndex = QModelIndex();
    if (index.isValid()) {
        auto moveInJson =
            model.itemFromIndex(index)->data().value<QJsonObject>();
        readPollJson(moveInJson);
        /*qDebug() << "In:" << moveInJson; */
        curPollIndex = index;
    }

    /*qDebug() << "curIndex" << index.row() << curPollIndex.row(); */
}

void LootTableEditorDock::checkPolls() {
    auto indexes = ui->pollListView->selectionModel()->selectedIndexes();

    ui->deletePoolButton->setDisabled(indexes.isEmpty());
    ui->pollEditor->setDisabled(indexes.isEmpty());
}

QJsonObject LootTableEditorDock::writePollJson() {
    QJsonObject root;

    if (ui->bonusRollsCheck->isChecked())
        root.insert("bonus_rolls", ui->bonusRollsInput->toJson());
    root.insert("rolls", ui->rollsInput->toJson());
    return root;
}

void LootTableEditorDock::readPollJson(const QJsonObject &root) {
    ui->rollsInput->fromJson(root.value("rolls"));
    ui->bonusRollsCheck->setChecked(root.contains("bonus_rolls"));
    if (root.contains("bonus_rolls"))
        ui->bonusRollsInput->fromJson(root.value("bonus_rolls"));
}
