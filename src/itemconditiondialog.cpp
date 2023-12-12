#include "itemconditiondialog.h"
#include "ui_itemconditiondialog.h"

#include "numberproviderdelegate.h"
#include "inventoryslot.h"

#include "game.h"
#include "platforms/windows_specific.h"

#include <QJsonObject>

ItemConditionDialog::ItemConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::ItemConditionDialog) {
    ui->setupUi(this);

    from_1_17 = Game::version() >= Game::v1_17;

    ui->itemSlot->setAcceptTag(false);
    ui->itemSlot->setAcceptMultiple(from_1_17);

    initComboModelView("effect", potionsModel, ui->potionCombo);
    initComboModelView("enchantment", enchantmentsModel, ui->enchant_combo,
                       false);
    ui->stored_combo->setModel(&enchantmentsModel);

    initTable(ui->enchant_table);
    initTable(ui->stored_table);

    connect(ui->itemRadio, &QRadioButton::toggled,
            ui->itemSlot, &InventorySlot::setEnabled);
    connect(ui->tagRadio, &QRadioButton::toggled,
            ui->itemTagEdit, &QLineEdit::setEnabled);
    connect(ui->enchant_addBtn, &QPushButton::clicked,
            this, &ItemConditionDialog::onAddedEnchant);
    connect(ui->stored_addBtn, &QPushButton::clicked,
            this, &ItemConditionDialog::onAddedStoredEnchant);

    if (from_1_17) {
        m_controller.addMapping(QStringLiteral("items"), ui->itemRadio,
                                ui->itemSlot);
    } else {
        m_controller.addMapping(QStringLiteral("item"), ui->itemRadio,
                                ui->itemSlot);
    }
    m_controller.addMapping(QStringLiteral("tag"), ui->tagRadio,
                            ui->itemTagEdit);
    m_controller.addMapping(QStringLiteral("count"), ui->countInput);
    m_controller.addMapping(QStringLiteral("durability"), ui->durabilityInput);
    m_controller.addMapping(QStringLiteral("nbt"), ui->NBTEdit);
    m_controller.addMapping(QStringLiteral("potion"), ui->potionCombo);

    adjustSize();
}

ItemConditionDialog::~ItemConditionDialog() {
    delete ui;
}

QJsonObject ItemConditionDialog::toJson() const {
    QJsonObject root;

    m_controller.putValueTo(root, {});

    QJsonArray enchantments;
    for (auto row = 0; row < ui->enchant_table->rowCount(); ++row) {
        auto id =
            ui->enchant_table->item(row, 0)->data(Qt::UserRole + 1).toString();
        auto levels = ui->enchant_table->item(row, 1)->
                      data(ExtendedRole::NumberProviderRole).toJsonValue();
        enchantments.push_back(QJsonObject({ { QStringLiteral("enchantment"),
                                               id },
                                               { QStringLiteral("levels"),
                                                 levels } }));
    }
    if (!enchantments.isEmpty())
        root.insert(QStringLiteral("enchantments"), enchantments);

    QJsonArray stored_enchantments;
    for (auto row = 0; row < ui->stored_table->rowCount(); ++row) {
        auto id =
            ui->stored_table->item(row, 0)->data(Qt::UserRole + 1).toString();
        auto levels = ui->stored_table->item(row, 1)->
                      data(ExtendedRole::NumberProviderRole).toJsonValue();
        stored_enchantments.push_back
            (QJsonObject({ { QStringLiteral("enchantment"), id },
                             { QStringLiteral("levels"), levels } }));
    }
    if (!stored_enchantments.isEmpty())
        root.insert(QStringLiteral("stored_enchantments"), stored_enchantments);

    return root;
}

void ItemConditionDialog::fromJson(const QJsonObject &value) {
    if (value.isEmpty())
        return;

    m_controller.setValueFrom(value, {});

    if (value.contains(QStringLiteral("enchantments"))) {
        tableFromJson(value[QStringLiteral("enchantments")].toArray(),
                      ui->enchant_table);
    }

    if (value.contains(QStringLiteral("stored_enchantments"))) {
        tableFromJson(value[QStringLiteral("stored_enchantments")].toArray(),
                      ui->stored_table);
    }
}

void ItemConditionDialog::onAddedEnchant() {
    if ((ui->enchant_levelInput->minValue() == 0 &&
         !ui->enchant_levelInput->isCurrentlyUnset())
        || (ui->enchant_levelInput->maxValue() == 0 &&
            !ui->enchant_levelInput->isCurrentlyUnset())) {
        return;
    }
    QString enchantmentText = ui->enchant_combo->currentText();
    if (!ui->enchant_table->findItems(enchantmentText,
                                      Qt::MatchExactly).isEmpty())
        return;

    auto *enchantItem = new QTableWidgetItem(enchantmentText);
    enchantItem->setData(Qt::UserRole + 1,
                         ui->enchant_combo->currentData(Qt::UserRole + 1));
    enchantItem->setFlags(enchantItem->flags() & ~(Qt::ItemIsEditable));

    auto *levelsItem = new QTableWidgetItem();
    auto  json       = ui->enchant_levelInput->toJson();
    levelsItem->setData(ExtendedRole::NumberProviderRole, json);
    appendRowToTableWidget(ui->enchant_table, { enchantItem, levelsItem });
}

void ItemConditionDialog::onAddedStoredEnchant() {
    if ((ui->stored_levelInput->minValue() == 0 &&
         !ui->stored_levelInput->isCurrentlyUnset())
        || (ui->stored_levelInput->maxValue() == 0 &&
            !ui->stored_levelInput->isCurrentlyUnset())) {
        return;
    }
    QString enchantmentText = ui->stored_combo->currentText();
    if (!ui->stored_table->findItems(enchantmentText,
                                     Qt::MatchExactly).isEmpty())
        return;

    auto *enchantItem = new QTableWidgetItem(enchantmentText);
    enchantItem->setData(Qt::UserRole + 1,
                         ui->stored_combo->currentData(Qt::UserRole + 1));
    enchantItem->setFlags(enchantItem->flags() & ~(Qt::ItemIsEditable));

    auto *levelsItem = new QTableWidgetItem();
    auto  json       = ui->stored_levelInput->toJson();
    levelsItem->setData(ExtendedRole::NumberProviderRole, json);
    appendRowToTableWidget(ui->stored_table, { enchantItem, levelsItem });
}

void ItemConditionDialog::initTable(QTableWidget *table) {
    auto *delegate = new NumberProviderDelegate(this);

    delegate->setInputModes(NumberProvider::ExactAndRange);
    table->setItemDelegateForColumn(1, delegate);
    table->installEventFilter(new ViewEventFilter(this));
}

void ItemConditionDialog::tableFromJson(const QJsonArray &jsonArr,
                                        QTableWidget *table) {
    for (const auto &enchantment : jsonArr) {
        auto enchantObj = enchantment.toObject();
        if (enchantObj.isEmpty()) continue;
        if (!enchantObj.contains(QStringLiteral("enchantment"))) {
            continue;
        }
        auto enchantId = enchantObj[QStringLiteral("enchantment")].toString();
        if (!enchantId.contains(QStringLiteral(":")))
            enchantId = QStringLiteral("minecraft:") + enchantId;
        auto indexes = enchantmentsModel.match(
            enchantmentsModel.index(0, 0), Qt::UserRole + 1, enchantId);
        if (indexes.isEmpty()) continue;

        auto *enchantItem = new QTableWidgetItem();
        enchantItem->setData(Qt::UserRole + 1, enchantId);
        enchantItem->setText(indexes[0].data(Qt::DisplayRole).toString());
        enchantItem->setFlags(enchantItem->flags() & ~(Qt::ItemIsEditable));

        auto *levelsItem = new QTableWidgetItem();
        auto  json       = ui->stored_levelInput->toJson();
        levelsItem->setData(ExtendedRole::NumberProviderRole,
                            enchantObj.value(QStringLiteral("levels")));
        appendRowToTableWidget(table, { enchantItem, levelsItem });
    }
}
