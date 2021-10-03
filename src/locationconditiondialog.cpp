#include "locationconditiondialog.h"
#include "ui_locationconditiondialog.h"

#include "mainwindow.h"
#include "globalhelpers.h"
#include "inventoryitem.h"

#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

LocationConditionDialog::LocationConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::LocationConditionDialog) {
    ui->setupUi(this);

    const auto typeFlags = NumberProvider::Exact
                           | NumberProvider::Range;

    ui->xInput->setModes(typeFlags);
    ui->xInput->minimizeMinLimit();
    ui->yInput->setModes(typeFlags);
    ui->yInput->minimizeMinLimit();
    ui->zInput->setModes(typeFlags);
    ui->zInput->minimizeMinLimit();
    ui->lightInput->setModes(typeFlags);
    ui->lightInput->setMaxLimit(15);

    initComboModelView("biome", biomesModel, ui->biomeCombo);
    initComboModelView("dimension", dimensionsModel, ui->dimensionCombo);
    initComboModelView("feature", featuresModel, ui->featureCombo);

    connect(ui->addstateBtn, &QPushButton::clicked,
            this, &LocationConditionDialog::onAddedState);
    initBlockGroup();
    initFluidGroup();
    ui->stateTable->installEventFilter(&viewFilter);

    adjustSize();
}

LocationConditionDialog::~LocationConditionDialog() {
    delete ui;
}

QJsonObject LocationConditionDialog::toJson() const {
    QJsonObject root;
    QJsonObject position;

    if (!ui->xInput->isCurrentlyUnset())
        position.insert("x", ui->xInput->toJson());
    if (!ui->yInput->isCurrentlyUnset())
        position.insert("y", ui->yInput->toJson());
    if (!ui->zInput->isCurrentlyUnset())
        position.insert("z", ui->zInput->toJson());
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
        root.insert(QStringLiteral("light"), { { "light",
                        ui->lightInput->toJson() } });

    if (ui->blockGroup->isChecked()) {
        QJsonObject block;
        if (ui->block_blockRadio->isChecked()) {
            auto invItem = ui->blockCombo->currentData(Qt::UserRole + 1)
                           .value<InventoryItem>();
            block.insert(QStringLiteral("block"), invItem.getNamespacedID());
        } else if (!ui->blockTagEdit->text().isEmpty()) {
            block.insert(QStringLiteral("tag"), ui->blockTagEdit->text());
        }
        if (!ui->blockNBTEdit->text().isEmpty())
            block.insert(QStringLiteral("nbt"), ui->blockNBTEdit->text());
        QJsonObject blockStates = jsonFromStateTable(ui->stateTable);
        if (!blockStates.isEmpty() && ui->stateGroup->isChecked())
            block.insert(QStringLiteral("state"), blockStates);
        root.insert(QStringLiteral("block"), block);
    } else if (ui->fluidGroup->isChecked()) {
        QJsonObject fluid;
        if (ui->fluid_fluidRadio->isChecked()) {
            fluid.insert(QStringLiteral("fluid"),
                         ui->fluidCombo->currentData(
                             Qt::UserRole + 1).toString());
        } else if (!ui->fluidTagEdit->text().isEmpty()) {
            fluid.insert(QStringLiteral("tag"), ui->fluidTagEdit->text());
        }
        QJsonObject fluidStates = jsonFromStateTable(ui->stateTable);;
        if (!fluidStates.isEmpty() && ui->stateGroup->isChecked())
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
    if (value.contains(QStringLiteral("light"))) {
        auto light = value[QStringLiteral("light")].toObject();
        if (light.contains(QStringLiteral("light")))
            ui->lightInput->fromJson(light[QStringLiteral("light")]);
    }

    ui->blockGroup->setChecked(value.contains(QStringLiteral("block")));
    ui->fluidGroup->setChecked(value.contains(QStringLiteral("fluid")));
    if (value.contains(QStringLiteral("block"))) {
        QJsonObject block = value[QStringLiteral("block")].toObject();
        ui->block_tagRadio->setChecked(block.contains(QStringLiteral("tag")));
        if (block.contains(QStringLiteral("block"))) {
            InventoryItem invItem(block[QStringLiteral("block")].toString());
            /*qDebug() << invItem; */
            setupComboFrom(ui->blockCombo, QVariant::fromValue(invItem));
        } else if (block.contains(QStringLiteral("tag"))) {
            if (block.contains(QStringLiteral("tag")))
                ui->blockTagEdit->setText(block[QStringLiteral(
                                                    "tag")].toString());
        }
        if (block.contains(QStringLiteral("nbt")))
            ui->blockNBTEdit->setText(block[QStringLiteral("nbt")].toString());

        if (block.contains(QStringLiteral("state"))) {
            ui->stateGroup->setChecked(true);
            QJsonObject state = block[QStringLiteral("state")].toObject();
            setupStateTableFromJson(ui->stateTable, state);
        }
    } else if (value.contains(QStringLiteral("fluid"))) {
        QJsonObject fluid = value[QStringLiteral("fluid")].toObject();
        ui->fluid_tagRadio->setChecked(fluid.contains(QStringLiteral("tag")));
        if (fluid.contains(QStringLiteral("fluid"))) {
            InventoryItem invItem(fluid[QStringLiteral("fluid")].toString());
            setupComboFrom(ui->fluidCombo, QVariant::fromValue(invItem));
        } else if (fluid.contains(QStringLiteral("tag"))) {
            ui->blockTagEdit->setText(fluid[QStringLiteral("tag")].toString());
        }
        if (fluid.contains(QStringLiteral("state"))) {
            ui->stateGroup->setChecked(true);
            QJsonObject state = fluid[QStringLiteral("state")].toObject();
            setupStateTableFromJson(ui->stateTable, state);
        }
    }
}

void LocationConditionDialog::setupStateTableFromJson(QTableWidget *table,
                                                      const QJsonObject &json) {
    for (const auto &key : json.keys()) {
        auto *stateItem = new QTableWidgetItem();
        stateItem->setText(key);
        auto *valueItem = new QTableWidgetItem();
        valueItem->setText(
            Glhp::variantToStr(json[key].toVariant()));
        appendRowToTableWidget(table, { stateItem, valueItem });
    }
}

QJsonObject LocationConditionDialog::jsonFromStateTable(
    const QTableWidget *table) {
    QJsonObject states;

    for (auto row = 0; row < table->rowCount(); ++row) {
        QString state = table->item(row, 0)->text();
        auto    value =
            table->item(row, 1)->data(Qt::DisplayRole).toString();
        states.insert(state,
                      Glhp::strToVariant(value).toJsonValue());
    }
    return states;
}

void LocationConditionDialog::onAddedState() {
    if (ui->stateEdit->text().isEmpty()
        || ui->stateValueEdit->text().isEmpty())
        return;

    QTableWidgetItem *stateItem = new QTableWidgetItem(
        ui->stateEdit->text());
    QTableWidgetItem *valueItem = new QTableWidgetItem(
        ui->stateValueEdit->text());

    appendRowToTableWidget(ui->stateTable, { stateItem, valueItem });
}

void LocationConditionDialog::initBlockGroup() {
    ui->blockGroup->setChecked(false);

    /*blocksModel.appendRow(new QStandardItem(tr("(not set)"))); */
    auto blocksInfo = MainWindow::getMCRInfo(QStringLiteral("block"));
    for (const auto &key : blocksInfo.keys()) {
        InventoryItem  invItem(key);
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(invItem.getPixmap()));
        if (invItem.getName().isEmpty()) {
            auto name = blocksInfo.value(key).toMap().value("name").toString();
            item->setText(name);
        } else {
            item->setText(invItem.getName());
        }
        item->setData(QVariant::fromValue(invItem), Qt::UserRole + 1);
        blocksModel.appendRow(item);
    }
    ui->blockCombo->setModel(&blocksModel);

    connect(ui->blockGroup, &QGroupBox::toggled, [this](bool checked) {
        if (ui->fluidGroup->isChecked() && checked)
            ui->fluidGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });
    connect(ui->block_blockRadio, &QRadioButton::toggled,
            ui->blockCombo, &QWidget::setEnabled);
    connect(ui->block_tagRadio, &QRadioButton::toggled,
            ui->blockTagEdit, &QWidget::setEnabled);
}

void LocationConditionDialog::initFluidGroup() {
    ui->fluidGroup->setChecked(false);

    initComboModelView(QStringLiteral("fluid"), fluidsModel, ui->fluidCombo,
                       false);

    connect(ui->fluidGroup, &QGroupBox::toggled, [this](bool checked) {
        if (ui->blockGroup->isChecked() && checked)
            ui->blockGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });
    connect(ui->fluid_fluidRadio, &QRadioButton::toggled,
            ui->fluidCombo, &QWidget::setEnabled);
    connect(ui->fluid_tagRadio, &QRadioButton::toggled,
            ui->fluidTagEdit, &QWidget::setEnabled);
}
