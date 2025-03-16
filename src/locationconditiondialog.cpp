#include "locationconditiondialog.h"
#include "ui_locationconditiondialog.h"

#include "globalhelpers.h"

#include "game.h"

#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>

LocationConditionDialog::LocationConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(), ui(new Ui::LocationConditionDialog) {
    ui->setupUi(this);

    ui->xInput->minimizeMinLimit();
    ui->yInput->minimizeMinLimit();
    ui->zInput->minimizeMinLimit();

    m_biomeModel.setInfo(QStringLiteral("biome"));
    m_biomeModel.setDatapackCategory(QStringLiteral("worldgen/biome"));
    ui->biomeSelector->setIdModel(&m_biomeModel);
    m_dimensionModel.setInfo(QStringLiteral("dimension"));
    m_dimensionModel.setDatapackCategory(QStringLiteral("dimension"));
    ui->dimensionCombo->setModel(&m_dimensionModel);
    if (Game::version() >= Game::v1_19) {
        m_featureModel.setRegistry(QStringLiteral("worldgen/structure"));
        m_featureModel.setDatapackCategory(QStringLiteral("worldgen/structure"));
    } else if (Game::version() == Game::v1_18_2) {
        m_featureModel.setRegistry(QStringLiteral(
                                       "worldgen/configured_structure_feature"));
        m_featureModel.setDatapackCategory(
            QStringLiteral("worldgen/configured_structure_feature"));
    } else {
        m_featureModel.setInfo(QStringLiteral("feature"));
    }
    ui->structureSelector->setIdModel(&m_featureModel);

    if (UNTIL_VER(v1_16)) {
        ui->smokeyCheck->hide();
    }
    if (UNTIL_VER(v1_21)) {
        ui->canSeeSkyCheck->hide();
    }

    initBlockGroup();
    initFluidGroup();

    ui->blockSlot->setAcceptItemsOrTag();
    if (UNTIL_VER(v1_20_6)) {
        if (UNTIL_VER(v1_17)) {
            ui->blockSlot->setAcceptMultiple(false);
        }

        ui->biomeSelector->setModes(IdTagSelector::Id);
        ui->structureSelector->setModes(IdTagSelector::Id);
    } else {
        m_biomeModel.setOptionalItem(false);
        ui->biomeSelector->setTagModel(
            new GameInfoModel(this, "tag/worldgen/biome",
                              GameInfoModel::Registry,
                              GameInfoModel::PrependPrefix,
                              "tags/worldgen/biome"));

        m_featureModel.setOptionalItem(false);
        ui->structureSelector->setTagModel(
            new GameInfoModel(this, "tag/worldgen/structure",
                              GameInfoModel::Registry,
                              GameInfoModel::PrependPrefix,
                              "tags/worldgen/structure"));
    }
    ui->blockSlot->setSelectCategory(InventorySlot::SelectCategory::Blocks);

    m_controller.addMappings({
        { "position", new DWCRecord{
              { "x", ui->xInput },
              { "y", ui->yInput },
              { "z", ui->zInput },
          } },
        { FROM_VER(v1_20_6) ? "biomes" : "biome", ui->biomeSelector },
        { "dimension", ui->dimensionCombo },
        { "light", new DWCRecord{
              { "light", ui->lightInput },
          } },
        { "block", new DWCRecord{
              { FROM_VER(v1_17) ? "blocks" : "block", ui->blockSlot, false,
                FROM_VER(v1_20_6) },
              { "nbt", ui->blockNbtEdit },
              { "state", ui->stateTable },
          } },
        { "fluid", new DWCRecord{
              { FROM_VER(v1_17) ? "fluids" : "fluid", ui->fluidSelector, false,
                FROM_VER(v1_20_6) },
              // The conflict will be handled in the fromJson() and toJson() methods
              { "state", ui->stateTable },
          } },
    });

    if (FROM_VER(v1_20_6)) {
        m_controller.addMapping("structures", ui->structureSelector);
    } else if (FROM_VER(v1_19)) {
        m_controller.addMapping("structure", ui->structureSelector);
    } else {
        m_controller.addMapping("feature", ui->structureSelector);
    }

    if (FROM_VER(v1_21)) {
        m_controller.addMapping("can_see_sky", ui->canSeeSkyCheck);
    }
    if (FROM_VER(v1_16)) {
        m_controller.addMapping("smokey", ui->smokeyCheck);
    }

    // adjustSize();
}

LocationConditionDialog::~LocationConditionDialog() {
    delete ui;
}

QJsonObject LocationConditionDialog::toJson() const {
    QJsonObject root;

    m_controller.putValueTo(root, {});

    if (root.value(QStringLiteral("block")).toObject().contains("state")
        && root.value(QStringLiteral("fluid")).toObject().contains("state")) {
        if (ui->blockGroup->isChecked()) {
            QJsonValueRef &&ref    = root[QStringLiteral("fluid")];
            auto          &&refObj = ref.toObject();
            refObj.remove("state");
            if (refObj.isEmpty()) {
                root.remove(QStringLiteral("fluid"));
            } else {
                ref = refObj;
            }
        } else {
            QJsonValueRef &&ref    = root[QStringLiteral("block")];
            auto          &&refObj = ref.toObject();
            refObj.remove("state");
            if (refObj.isEmpty()) {
                root.remove(QStringLiteral("block"));
            } else {
                ref = refObj;
            }
        }
    }

    return root;
}

void LocationConditionDialog::fromJson(const QJsonObject &value) {
    ui->blockGroup->setChecked(!value.value(QStringLiteral(
                                                "block")).toObject().isEmpty());
    ui->fluidGroup->setChecked(!value.value(QStringLiteral(
                                                "fluid")).toObject().isEmpty());

    if (value.value(QStringLiteral("block")).toObject().contains("state")
        || value.value(QStringLiteral("fluid")).toObject().contains("state")) {
        ui->stateGroup->setChecked(true);
    }

    m_controller.setValueFrom(value, {});
}

void LocationConditionDialog::initBlockGroup() {
    ui->blockGroup->setChecked(false);

    connect(ui->blockGroup, &QGroupBox::toggled, this, [this](bool checked) {
        if (ui->fluidGroup->isChecked() && checked)
            ui->fluidGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });

    ui->stateTable->appendColumnMapping({}, ui->stateEdit);
    ui->stateTable->appendColumnMapping({}, ui->stateValueCombo);
}

void LocationConditionDialog::initFluidGroup() {
    ui->fluidGroup->setChecked(false);

    m_fluidModel.setInfo(QStringLiteral("fluid"), GameInfoModel::PrependPrefix);
    ui->fluidSelector->setIdModel(&m_fluidModel);
    if (UNTIL_VER(v1_20_6)) {
        ui->fluidSelector->setModes(IdTagSelector::Id | IdTagSelector::Tag);
    }

    connect(ui->fluidGroup, &QGroupBox::toggled, this, [this](bool checked) {
        if (ui->blockGroup->isChecked() && checked)
            ui->blockGroup->setChecked(false);
        ui->stateGroup->setEnabled(checked);
    });
}

void LocationConditionDialog::LocationConditionDialog::setupStateTableFromJson(
    QTableWidget *table,
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

QJsonObject LocationConditionDialog::LocationConditionDialog::jsonFromStateTable(
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
