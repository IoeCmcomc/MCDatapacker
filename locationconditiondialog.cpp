#include "locationconditiondialog.h"
#include "ui_locationconditiondialog.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QJsonObject>
#include <QJsonArray>

LocationConditionDialog::LocationConditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocationConditionDialog) {
    ui->setupUi(this);

    const auto typeFlags = ExtendedNumericInput::Exact
                           | ExtendedNumericInput::Range;

    ui->xInput->setTypes(typeFlags);
    ui->yInput->setTypes(typeFlags);
    ui->zInput->setTypes(typeFlags);
    ui->lightInput->setTypes(typeFlags);
    ui->lightInput->setGeneralMinimum(0);
    ui->lightInput->setGeneralMaximum(15);

    initComboModelView("biomes", biomesModel, ui->biomeCombo);
    initComboModelView("dimensions", dimensionsModel, ui->dimensionCombo);

    featuresModel.appendRow(new QStandardItem(tr("(not selected)")));
    auto info = MainWindow::readMCRInfo("features");
    for (auto key : info.keys()) {
        QStandardItem *item = new QStandardItem();
        item->setText(info.value(key).toString());
        item->setData(key);
        featuresModel.appendRow(item);
    }
    ui->featureCombo->setModel(&featuresModel);

    initBlockGroup();
    initFluidGroup();

    connect(ui->xCheck, &QCheckBox::toggled, [ = ](bool checked) {
        ui->xInput->setEnabled(checked);
    });
    connect(ui->yCheck, &QCheckBox::toggled, [ = ](bool checked) {
        ui->yInput->setEnabled(checked);
    });
    connect(ui->zCheck, &QCheckBox::toggled, [ = ](bool checked) {
        ui->zInput->setEnabled(checked);
    });
    connect(ui->lightCheck, &QCheckBox::toggled, [ = ](bool checked) {
        ui->lightInput->setEnabled(checked);
    });

    adjustSize();
}

LocationConditionDialog::~LocationConditionDialog() {
    delete ui;
}

QJsonObject LocationConditionDialog::toJson() const {
    QJsonObject root;
    QJsonObject position;

    if (ui->xCheck->isChecked())
        position.insert("x", ui->xInput->toJson());
    if (ui->yCheck->isChecked())
        position.insert("y", ui->yInput->toJson());
    if (ui->zCheck->isChecked())
        position.insert("z", ui->zInput->toJson());
    if (!position.isEmpty())
        root.insert("position", position);

    if (ui->biomeCombo->currentIndex() != 0)
        root.insert("biome", ui->biomeCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->dimensionCombo->currentIndex() != 0)
        root.insert("dimension", ui->dimensionCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->featureCombo->currentIndex() != 0)
        root.insert("feature", ui->featureCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->lightCheck->isChecked())
        root.insert("light", ui->lightInput->toJson());

    if (ui->blockGroup->isChecked()) {
        QJsonObject block;
        auto        invItem =
            ui->blockCombo->currentData(Qt::UserRole + 1).value<MCRInvItem>();
        block.insert("block", invItem.getNamespacedID());
        if (!ui->blockTagEdit->text().isEmpty())
            block.insert("tag", ui->blockTagEdit->text());
        if (!ui->blockNBTEdit->text().isEmpty())
            block.insert("nbt", ui->blockNBTEdit->text());
        QJsonObject blockStates;
        for (auto row = 0; row < blockStatesModel.rowCount(); ++row) {
            QString state = blockStatesModel.item(row, 0)->text();
            auto    value =
                blockStatesModel.item(row, 1)->data(Qt::DisplayRole).toString();
            blockStates.insert(state,
                               GlobalHelpers::strToVariant(value).toJsonValue());
        }
        if (!blockStates.isEmpty())
            block.insert("state", blockStates);
        root.insert("block", block);
    }

    if (ui->fluidGroup->isChecked()) {
        QJsonObject fluid;
        fluid.insert("fluid",
                     ui->fluidCombo->currentData(Qt::UserRole + 1).toString());
        if (!ui->fluidTagEdit->text().isEmpty())
            fluid.insert("tag", ui->fluidTagEdit->text());
        QJsonObject fluidStates;
        for (auto row = 0; row < fluidStatesModel.rowCount(); ++row) {
            QString state = fluidStatesModel.item(row, 0)->text();
            auto    value =
                fluidStatesModel.item(row, 1)->data(Qt::DisplayRole).toString();
            fluidStates.insert(state,
                               GlobalHelpers::strToVariant(value).toJsonValue());
        }
        if (!fluidStates.isEmpty())
            fluid.insert("state", fluidStates);
        root.insert("fluid", fluid);
    }

    return root;
}

void LocationConditionDialog::fromJson(const QJsonObject &value) {
    if (value.contains("position")) {
        QJsonObject position = value["position"].toObject();
        ui->xCheck->setChecked(position.contains("x"));
        if (position.contains("x"))
            ui->xInput->fromJson(position["x"]);
        ui->yCheck->setChecked(position.contains("y"));
        if (position.contains("y"))
            ui->yInput->fromJson(position["y"]);
        ui->zCheck->setChecked(position.contains("z"));
        if (position.contains("z"))
            ui->zInput->fromJson(position["z"]);
    }

    if (value.contains("biome"))
        setupComboFrom(ui->biomeCombo, value["biome"]);
    if (value.contains("dimension"))
        setupComboFrom(ui->biomeCombo, value["biome"]);
    if (value.contains("feature"))
        setupComboFrom(ui->biomeCombo, value["biome"]);
    ui->lightCheck->setChecked(value.contains("light"));
    if (value.contains("light"))
        ui->lightInput->fromJson(value["x"]);

    ui->blockGroup->setChecked(value.contains("block"));
    if (value.contains("block")) {
        QJsonObject block = value["block"].toObject();
        if (block.contains("block")) {
            setupComboFrom(ui->blockCombo, block["block"].toVariant());
            if (block.contains("tag"))
                ui->blockTagEdit->setText(block["tag"].toString());
            if (block.contains("nbt"))
                ui->blockNBTEdit->setText(block["nbt"].toString());

            if (block.contains("state")) {
                QJsonObject state = block["state"].toObject();
                for (auto key : state.keys()) {
                    QStandardItem *stateItem = new QStandardItem();
                    stateItem->setText(key);
                    stateItem->setToolTip(deletiveToolTip);
                    QStandardItem *valueItem = new QStandardItem();
                    valueItem->setText(
                        GlobalHelpers::variantToStr(state[key].toVariant()));
                    valueItem->setToolTip(deletiveToolTip);
                    blockStatesModel.appendRow({ stateItem, valueItem });
                }
            }
        }
    }

    ui->fluidGroup->setChecked(value.contains("fluid"));
    if (value.contains("fluid")) {
        QJsonObject block = value["fluid"].toObject();
        if (block.contains("fluid")) {
            setupComboFrom(ui->blockCombo, block["fluid"].toVariant());
            if (block.contains("tag"))
                ui->blockTagEdit->setText(block["tag"].toString());

            if (block.contains("state")) {
                QJsonObject state = block["state"].toObject();
                for (auto key : state.keys()) {
                    QStandardItem *stateItem = new QStandardItem();
                    stateItem->setText(key);
                    stateItem->setToolTip(deletiveToolTip);
                    QStandardItem *valueItem = new QStandardItem();
                    valueItem->setText(
                        GlobalHelpers::variantToStr(state[key].toVariant()));
                    valueItem->setToolTip(deletiveToolTip);
                    fluidStatesModel.appendRow({ stateItem, valueItem });
                }
            }
        }
    }
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

void LocationConditionDialog::initNumericInput(ExtendedNumericInput *input,
                                               const int &min,
                                               const int &max) {
    input->setExactMinimum(min);
    input->setExactMaximum(max);
    input->setRangeMinimum(min);
    input->setRangeMaximum(max);
}

void LocationConditionDialog::initComboModelView(const QString &infoType,
                                                 QStandardItemModel &model,
                                                 QComboBox *combo,
                                                 bool optinal) {
    if (optinal)
        model.appendRow(new QStandardItem(tr("(not selected)")));
    auto info = MainWindow::readMCRInfo(infoType);
    for (auto key : info.keys()) {
        QStandardItem *item = new QStandardItem();
        item->setText(info.value(key).toString());
        item->setData("minecraft:" + key);
        model.appendRow(item);
    }
    combo->setModel(&model);
}

void LocationConditionDialog::initBlockGroup() {
    ui->blockGroup->setChecked(false);

    auto blocksInfo = MainWindow::getMCRInfo("block");
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

    QStandardItem *stateItem = new QStandardItem("State");
    stateItem->setToolTip("An state of the block.");
    QStandardItem *valuesItem = new QStandardItem("Value");
    valuesItem->setToolTip("A value of the state.");

    blockStatesModel.setHorizontalHeaderItem(0, stateItem);
    blockStatesModel.setHorizontalHeaderItem(1, valuesItem);

    ui->blockStatesView->setModel(&blockStatesModel);
    ui->blockStatesView->installEventFilter(&viewFilter);

    connect(ui->addBlockStateBtn, &QPushButton::clicked,
            this, &LocationConditionDialog::onAddedBlockState);
}

void LocationConditionDialog::initFluidGroup() {
    ui->fluidGroup->setChecked(false);

    initComboModelView("fluids", fluidsModel, ui->fluidCombo, false);

    QStandardItem *stateItem = new QStandardItem("State");
    stateItem->setToolTip("An state of the block.");
    QStandardItem *valuesItem = new QStandardItem("Value");
    valuesItem->setToolTip("A value of the state.");

    fluidStatesModel.setHorizontalHeaderItem(0, stateItem);
    fluidStatesModel.setHorizontalHeaderItem(1, valuesItem);

    ui->fluidStatesView->setModel(&fluidStatesModel);
    ui->fluidStatesView->installEventFilter(&viewFilter);

    connect(ui->addFluidStateBtn, &QPushButton::clicked,
            this, &LocationConditionDialog::onAddedFluidState);
}

void LocationConditionDialog::setupComboFrom(QComboBox *combo,
                                             const QVariant &vari,
                                             int role) {
    auto *model = qobject_cast<QStandardItemModel*>(combo->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i, 0)->data(role) == vari) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}
