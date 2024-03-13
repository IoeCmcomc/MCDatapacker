#include "locationconditiondialog.h"
#include "ui_locationconditiondialog.h"

#include "globalhelpers.h"
#include "inventoryslot.h"

#include "game.h"

#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

LocationConditionDialog::LocationConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(), ui(new Ui::LocationConditionDialog) {
    ui->setupUi(this);

    ui->xInput->minimizeMinLimit();
    ui->yInput->minimizeMinLimit();
    ui->zInput->minimizeMinLimit();

    // initComboModelView("biome", biomesModel, ui->biomeCombo);
    biomesModel.setSource(QStringLiteral("biome"), GameInfoModel::Info,
                          GameInfoModel::PrependPrefix |
                          GameInfoModel::HasOptionalItem);
    biomesModel.setDatapackCategory(QStringLiteral("worldgen/biome"));
    ui->biomeCombo->setModel(&biomesModel);
    initComboModelView("dimension", dimensionsModel, ui->dimensionCombo);
    if (Game::version() >= Game::v1_19) {
        initComboModelViewFromRegistry("worldgen/structure", featuresModel,
                                       ui->featureCombo);
    } else if (Game::version() == Game::v1_18_2) {
        initComboModelViewFromRegistry("worldgen/configured_structure_feature",
                                       featuresModel, ui->featureCombo);
    } else {
        initComboModelView("feature", featuresModel, ui->featureCombo);
    }

    if (Game::version() < Game::v1_16) {
        ui->smokeyCheck->hide();
    }

    from_1_17 = Game::version() >= Game::v1_17;
    ui->blockSlot->setAcceptMultiple(from_1_17);
    ui->blockSlot->setAcceptTag(false);
    ui->blockSlot->setSelectCategory(InventorySlot::SelectCategory::Blocks);

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
                        GameInfoModel::IdRole).toString());
    if (ui->dimensionCombo->currentIndex() != 0)
        root.insert(QStringLiteral("dimension"),
                    ui->dimensionCombo->currentData(
                        Qt::UserRole + 1).toString());
    if (ui->featureCombo->currentText() != 0) {
        if (Game::version() >= Game::v1_19) {
            root.insert(QStringLiteral("structure"),
                        ui->featureCombo->currentData(
                            Qt::UserRole + 1).toString());
        } else {
            root.insert(QStringLiteral("feature"),
                        ui->featureCombo->currentData(
                            Qt::UserRole + 1).toString());
        }
    }
    if (!ui->lightInput->isCurrentlyUnset())
        root.insert(QStringLiteral("light"), { { "light",
                        ui->lightInput->toJson() } });
    if (Game::version() >= QVersionNumber(1, 16)) {
        ui->smokeyCheck->insertToJsonObject(root, "smokey");
    }

    if (ui->blockGroup->isChecked()) {
        QJsonObject block;
        if (ui->block_blockRadio->isChecked()) {
            if (from_1_17) {
                QJsonArray blocks;
                for (int i = 0; i < ui->blockSlot->itemCount(); ++i) {
                    blocks << ui->blockSlot->itemNamespacedID(i);
                }
                if (!blocks.isEmpty()) {
                    block.insert(QStringLiteral("blocks"), blocks);
                }
            } else {
                block.insert(QStringLiteral("block"),
                             ui->blockSlot->itemNamespacedID());
            }
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
        QJsonObject fluidStates = jsonFromStateTable(ui->stateTable);
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
        setComboValueFrom(ui->biomeCombo, value[QStringLiteral("biome")]);
    if (value.contains(QStringLiteral("dimension")))
        setComboValueFrom(ui->dimensionCombo,
                          value[QStringLiteral("dimension")]);
    if (value.contains(QStringLiteral("feature"))) {
        if (Game::version() >= Game::v1_19) {
            setComboValueFrom(ui->featureCombo,
                              value[QStringLiteral("structure")]);
        } else {
            setComboValueFrom(ui->featureCombo,
                              value[QStringLiteral("feature")]);
        }
    }
    if (value.contains(QStringLiteral("light"))) {
        auto light = value[QStringLiteral("light")].toObject();
        if (light.contains(QStringLiteral("light")))
            ui->lightInput->fromJson(light[QStringLiteral("light")]);
    }
    if (Game::version() >= Game::v1_16) {
        ui->smokeyCheck->setupFromJsonObject(value, "smokey");
    }

    ui->blockGroup->setChecked(value.contains(QStringLiteral("block")));
    ui->fluidGroup->setChecked(value.contains(QStringLiteral("fluid")));
    if (value.contains(QStringLiteral("block"))) {
        QJsonObject block = value[QStringLiteral("block")].toObject();
        ui->block_tagRadio->setChecked(block.contains(QStringLiteral("tag")));
        ui->blockSlot->clearItems();
        if (from_1_17 && block.contains(QStringLiteral("blocks"))) {
            const auto &&blocks = block["blocks"].toArray();
            for (const auto &blockId: blocks) {
                ui->blockSlot->appendItem(blockId.toString());
            }
        } else if (block.contains(QStringLiteral("block"))) {
            InventoryItem invItem(block[QStringLiteral("block")].toString());
            ui->blockSlot->setItem(invItem);
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
            setComboValueFrom(ui->fluidCombo,
                              fluid[QStringLiteral("fluid")].toString());
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
    const auto &&keys = json.keys();

    for (const auto &key : keys) {
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

    connect(ui->blockGroup, &QGroupBox::toggled, this, [this](bool checked) {
        if (ui->fluidGroup->isChecked() && checked)
            ui->fluidGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });
    connect(ui->block_blockRadio, &QRadioButton::toggled,
            ui->blockSlot, &QWidget::setEnabled);
    connect(ui->block_tagRadio, &QRadioButton::toggled,
            ui->blockTagEdit, &QWidget::setEnabled);
}

void LocationConditionDialog::initFluidGroup() {
    ui->fluidGroup->setChecked(false);

    initComboModelView(QStringLiteral("fluid"), fluidsModel, ui->fluidCombo,
                       false);

    connect(ui->fluidGroup, &QGroupBox::toggled, this, [this](bool checked) {
        if (ui->blockGroup->isChecked() && checked)
            ui->blockGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });
    connect(ui->fluid_fluidRadio, &QRadioButton::toggled,
            ui->fluidCombo, &QWidget::setEnabled);
    connect(ui->fluid_tagRadio, &QRadioButton::toggled,
            ui->fluidTagEdit, &QWidget::setEnabled);
}
