#include "locationconditiondialog.h"
#include "ui_locationconditiondialog.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QJsonObject>
#include <QJsonArray>

LocationConditionDialog::LocationConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::LocationConditionDialog) {
    ui->setupUi(this);

    const auto typeFlags = NumericInput::Exact
                           | NumericInput::Range;

    ui->xInput->setTypes(typeFlags);
    ui->xInput->setGeneralMinimum(-999999999);
    ui->yInput->setTypes(typeFlags);
    ui->yInput->setGeneralMinimum(-999999999);
    ui->zInput->setTypes(typeFlags);
    ui->zInput->setGeneralMinimum(-999999999);
    ui->lightInput->setTypes(typeFlags);
    ui->lightInput->setGeneralMaximum(15);

    initComboModelView("biome", biomesModel, ui->biomeCombo);
    initComboModelView("dimension", dimensionsModel, ui->dimensionCombo);
    initComboModelView("feature", featuresModel, ui->featureCombo);

/*
      featuresModel.appendRow(new QStandardItem(tr("(not selected)")));
      auto info = MainWindow::readMCRInfo(QStringLiteral("feature"));
      for (auto key : info.keys()) {
          QStandardItem *item = new QStandardItem();
          item->setText(info.value(key).toString());
          item->setData(key);
          featuresModel.appendRow(item);
      }
      ui->featureCombo->setModel(&featuresModel);
 */

    initBlockGroup();
    initFluidGroup();

    adjustSize();
}

LocationConditionDialog::~LocationConditionDialog() {
    delete ui;
}

QJsonObject LocationConditionDialog::toJson() const {
    QJsonObject root;
    QJsonObject position;

    if (!ui->xInput->isCurrentlyUnset())
        position.insert(QStringLiteral("x"), ui->xInput->toJson());
    if (!ui->yInput->isCurrentlyUnset())
        position.insert(QStringLiteral("y"), ui->yInput->toJson());
    if (!ui->zInput->isCurrentlyUnset())
        position.insert(QStringLiteral("z"), ui->zInput->toJson());
    if (!position.isEmpty())
        root.insert(QStringLiteral("position"), position);

    if (ui->biomeCombo->currentIndex() != 0)
        root.insert(QStringLiteral("biome"), ui->biomeCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->dimensionCombo->currentIndex() != 0)
        root.insert(QStringLiteral("dimension"),
                    ui->dimensionCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->featureCombo->currentIndex() != 0)
        root.insert(QStringLiteral("feature"), ui->featureCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (!ui->lightInput->isCurrentlyUnset())
        root.insert(QStringLiteral("light"), ui->lightInput->toJson());

    if (ui->blockGroup->isChecked()) {
        QJsonObject block;
        auto        invItem =
            ui->blockCombo->currentData(Qt::UserRole + 1).value<MCRInvItem>();
        block.insert(QStringLiteral("block"), invItem.getNamespacedID());
        if (!ui->blockTagEdit->text().isEmpty())
            block.insert(QStringLiteral("tag"), ui->blockTagEdit->text());
        if (!ui->blockNBTEdit->text().isEmpty())
            block.insert(QStringLiteral("nbt"), ui->blockNBTEdit->text());
        QJsonObject blockStates = jsonFromTable(blockStatesModel);
        if (!blockStates.isEmpty())
            block.insert(QStringLiteral("state"), blockStates);
        root.insert(QStringLiteral("block"), block);
    }

    if (ui->fluidGroup->isChecked()) {
        QJsonObject fluid;
        fluid.insert(QStringLiteral("fluid"),
                     ui->fluidCombo->currentData(Qt::UserRole + 1).toString());
        if (!ui->fluidTagEdit->text().isEmpty())
            fluid.insert(QStringLiteral("tag"), ui->fluidTagEdit->text());
        QJsonObject fluidStates = jsonFromTable(fluidStatesModel);;
        if (!fluidStates.isEmpty())
            fluid.insert(QStringLiteral("state"), fluidStates);
        root.insert(QStringLiteral("fluid"), fluid);
    }

    return root;
}

void LocationConditionDialog::fromJson(const QJsonObject &value) {
    if (value.contains(QStringLiteral("position"))) {
        QJsonObject position = value[QStringLiteral("position")].toObject();
        if (position.contains(QStringLiteral("x")))
            ui->xInput->fromJson(position[QStringLiteral("x")]);
        if (position.contains(QStringLiteral("y")))
            ui->yInput->fromJson(position[QStringLiteral("y")]);
        if (position.contains(QStringLiteral("z")))
            ui->zInput->fromJson(position[QStringLiteral("z")]);
    }

    if (value.contains(QStringLiteral("biome")))
        setupComboFrom(ui->biomeCombo, value[QStringLiteral("biome")]);
    if (value.contains(QStringLiteral("dimension")))
        setupComboFrom(ui->dimensionCombo, value[QStringLiteral("dimension")]);
    if (value.contains(QStringLiteral("feature")))
        setupComboFrom(ui->featureCombo, value[QStringLiteral("feature")]);
    if (value.contains(QStringLiteral("light")))
        ui->lightInput->fromJson(value[QStringLiteral("light")]);

    ui->blockGroup->setChecked(value.contains(QStringLiteral("block")));
    if (value.contains(QStringLiteral("block"))) {
        QJsonObject block = value[QStringLiteral("block")].toObject();
        if (block.contains(QStringLiteral("block"))) {
            setupComboFrom(ui->blockCombo,
                           block[QStringLiteral("block")].toVariant());
            if (block.contains(QStringLiteral("tag")))
                ui->blockTagEdit->setText(block[QStringLiteral(
                                                    "tag")].toString());
            if (block.contains(QStringLiteral("nbt")))
                ui->blockNBTEdit->setText(block[QStringLiteral(
                                                    "nbt")].toString());

            setupTableFromJson(blockStatesModel, block);
        }
    }

    ui->fluidGroup->setChecked(value.contains(QStringLiteral("fluid")));
    if (value.contains(QStringLiteral("fluid"))) {
        QJsonObject fluid = value[QStringLiteral("fluid")].toObject();
        if (fluid.contains(QStringLiteral("fluid"))) {
            setupComboFrom(ui->blockCombo,
                           fluid[QStringLiteral("fluid")].toVariant());
            if (fluid.contains(QStringLiteral("tag")))
                ui->blockTagEdit->setText(fluid[QStringLiteral(
                                                    "tag")].toString());

            setupTableFromJson(blockStatesModel, fluid);
        }
    }
}

void LocationConditionDialog::setupTableFromJson(QStandardItemModel &model,
                                                 const QJsonObject &json) {
    if (json.contains(QStringLiteral("state"))) {
        QJsonObject state = json[QStringLiteral("state")].toObject();
        for (auto key : state.keys()) {
            QStandardItem *stateItem = new QStandardItem();
            stateItem->setText(key);
            stateItem->setToolTip(deletiveToolTip);
            QStandardItem *valueItem = new QStandardItem();
            valueItem->setText(
                GlobalHelpers::variantToStr(state[key].toVariant()));
            valueItem->setToolTip(deletiveToolTip);
            model.appendRow({ stateItem, valueItem });
        }
    }
}

QJsonObject LocationConditionDialog::jsonFromTable(
    const QStandardItemModel &model) {
    QJsonObject states;

    for (auto row = 0; row < model.rowCount(); ++row) {
        QString state = model.item(row, 0)->text();
        auto    value =
            model.item(row, 1)->data(Qt::DisplayRole).toString();
        states.insert(state,
                      GlobalHelpers::strToVariant(value).toJsonValue());
    }
    return states;
}

void LocationConditionDialog::onAddedBlockState() {
    if (ui->blockStateEdit->text().isEmpty()
        || ui->blockStateValueEdit->text().isEmpty())
        return;

    QStandardItem *stateItem = new QStandardItem(
        ui->blockStateEdit->text());
    stateItem->setToolTip(deletiveToolTip);
    QStandardItem *valueItem = new QStandardItem(
        ui->blockStateValueEdit->text());
    valueItem->setToolTip(deletiveToolTip);

    blockStatesModel.appendRow({ stateItem, valueItem });
}

void LocationConditionDialog::onAddedFluidState() {
    if (ui->fluidStateEdit->text().isEmpty()
        || ui->fluidStateValueEdit->text().isEmpty())
        return;

    QStandardItem *stateItem = new QStandardItem(
        ui->fluidStateEdit->text());
    stateItem->setToolTip(deletiveToolTip);
    QStandardItem *valueItem = new QStandardItem(
        ui->fluidStateValueEdit->text());
    valueItem->setToolTip(deletiveToolTip);

    fluidStatesModel.appendRow({ stateItem, valueItem });
}

void LocationConditionDialog::initBlockGroup() {
    ui->blockGroup->setChecked(false);

    auto blocksInfo = MainWindow::getMCRInfo(QStringLiteral("block"));
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
    ui->blockCombo->setModel(&blocksModel);

    QStandardItem *stateItem = new QStandardItem(tr("State"));
    stateItem->setToolTip(tr("An state of the block."));
    QStandardItem *valuesItem = new QStandardItem(tr("Value"));
    valuesItem->setToolTip(tr("A value of the state."));

    blockStatesModel.setHorizontalHeaderItem(0, stateItem);
    blockStatesModel.setHorizontalHeaderItem(1, valuesItem);

    ui->blockStatesView->setModel(&blockStatesModel);
    ui->blockStatesView->installEventFilter(&viewFilter);

    connect(ui->addBlockStateBtn, &QPushButton::clicked,
            this, &LocationConditionDialog::onAddedBlockState);
}

void LocationConditionDialog::initFluidGroup() {
    ui->fluidGroup->setChecked(false);

    initComboModelView(QStringLiteral("fluid"),
                       fluidsModel,
                       ui->fluidCombo,
                       false);

    QStandardItem *stateItem = new QStandardItem(tr("State"));
    stateItem->setToolTip(tr("An state of the block."));
    QStandardItem *valuesItem = new QStandardItem(tr("Value"));
    valuesItem->setToolTip(tr("A value of the state."));

    fluidStatesModel.setHorizontalHeaderItem(0, stateItem);
    fluidStatesModel.setHorizontalHeaderItem(1, valuesItem);

    ui->fluidStatesView->setModel(&fluidStatesModel);
    ui->fluidStatesView->installEventFilter(&viewFilter);

    connect(ui->addFluidStateBtn, &QPushButton::clicked,
            this, &LocationConditionDialog::onAddedFluidState);
}
