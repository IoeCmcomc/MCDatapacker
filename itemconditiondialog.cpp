#include "itemconditiondialog.h"
#include "ui_itemconditiondialog.h"

#include "mainwindow.h"
#include "extendednumericdelegate.h"
#include "mcrinvslot.h"

ItemConditionDialog::ItemConditionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemConditionDialog) {
    ui->setupUi(this);

    auto typeFlags = ExtendedNumericInput::Exact
                     | ExtendedNumericInput::Range;
    ui->countInput->setTypes(typeFlags);
    ui->countInput->setGeneralMinimum(0);
    ui->durabilityInput->setTypes(typeFlags);
    ui->durabilityInput->setGeneralMinimum(0);
    ui->enchant_levelInput->setTypes(typeFlags);
    ui->enchant_levelInput->setGeneralMinimum(0);
    ui->stored_levelInput->setTypes(typeFlags);
    ui->stored_levelInput->setGeneralMinimum(0);
    ui->itemSlot->setAcceptTag(false);
    ui->itemSlot->setAcceptMultiItems(false);

    potionsModel.appendRow(new QStandardItem(tr("(not selected)")));
    auto effectsInfo = MainWindow::readMCRInfo("effects");
    for (auto key : effectsInfo.keys()) {
        QStandardItem *item = new QStandardItem();
        item->setText(effectsInfo.value(key).toString());
        item->setIcon(QIcon(":minecraft/texture/effect/" + key + ".png"));
        item->setData("minecraft:" + key);
        potionsModel.appendRow(item);
    }
    ui->potionCombo->setModel(&potionsModel);

    auto enchantmentsInfo = MainWindow::readMCRInfo("enchantments");
    for (auto key : enchantmentsInfo.keys()) {
        QStandardItem *item = new QStandardItem();
        auto           map  = enchantmentsInfo.value(key).toMap();
        item->setText(map.value("name").toString());
        item->setData("minecraft:" + key);
        item->setToolTip(map.value("summary").toString());
        enchantmentsModel.appendRow(item);
    }
    ui->enchant_combo->setModel(&enchantmentsModel);
    ui->stored_combo->setModel(&enchantmentsModel);

    initModelView(itemEnchantModel, ui->enchant_listView);
    initModelView(storedEnchantModel, ui->stored_listView);

    connect(ui->enchant_addBtn, &QPushButton::clicked,
            this, &ItemConditionDialog::onAddedEnchant);
    connect(ui->stored_addBtn, &QPushButton::clicked,
            this, &ItemConditionDialog::onAddedStoredEnchant);
    connect(ui->itemSlot, &MCRInvSlot::itemChanged,
            this, &ItemConditionDialog::checkOK);

    checkOK();
    adjustSize();
}

ItemConditionDialog::~ItemConditionDialog() {
    delete ui;
}

QJsonObject ItemConditionDialog::toJson() const {
    QJsonObject root;

    root.insert("item", ui->itemSlot->getItem().getNamespacedID());
    root.insert("count", ui->countInput->toJson());
    if (ui->durabilityInput->getExactly() != 0)
        root.insert("durability", ui->durabilityInput->toJson());
    if (!ui->NBTEdit->text().isEmpty())
        root.insert("nbt", ui->NBTEdit->text());
    if (!ui->itemTagEdit->text().isEmpty())
        root.insert("tag", ui->itemTagEdit->text());
    if (ui->potionCombo->currentIndex() != 0)
        root.insert("potion", ui->potionCombo->currentData(
                        Qt::UserRole + 1).toString());

    QJsonArray enchantments;
    for (auto row = 0; row < itemEnchantModel.rowCount(); ++row) {
        auto id =
            itemEnchantModel.item(row, 0)->data().toString();
        auto levels = itemEnchantModel.item(row, 1)->
                      data(Qt::DisplayRole).toJsonValue();
        enchantments.push_back(QJsonObject({ { "enchantment", id },
                                               { "levels", levels } }));
    }
    if (!enchantments.isEmpty())
        root.insert("enchantments", enchantments);

    QJsonArray stored_enchantments;
    for (auto row = 0; row < storedEnchantModel.rowCount(); ++row) {
        auto id =
            storedEnchantModel.item(row, 0)->data().toString();
        auto levels = storedEnchantModel.item(row, 1)->
                      data(Qt::DisplayRole).toJsonValue();
        stored_enchantments.push_back(QJsonObject({ { "enchantment", id },
                                                      { "levels", levels } }));
    }
    if (!stored_enchantments.isEmpty())
        root.insert("stored_enchantments", stored_enchantments);

    return root;
}

void ItemConditionDialog::fromJson(const QJsonObject &value) {
    if (value.isEmpty())
        return;

    auto itemId = value.value("item").toString();
    if (MCRInvItem(itemId).getName().isEmpty())
        return;

    ui->itemSlot->setItem(MCRInvItem(itemId));
    if (value.contains("count"))
        ui->countInput->fromJson(value["count"]);
    if (value.contains("durability"))
        ui->durabilityInput->fromJson(value["durability"]);
    if (value.contains("nbt"))
        ui->NBTEdit->setText(value["nbt"].toString());
    if (value.contains("tag"))
        ui->itemTagEdit->setText(value["tag"].toString());
    if (value.contains("potion")) {
        auto potionId = value["potion"].toString();
        for (int i = 0; i < potionsModel.rowCount(); ++i) {
            if (potionsModel.item(i, 0)->data().toString() == potionId) {
                ui->potionCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    if (value.contains("enchantments")) {
        tableFromJson(value["enchantments"].toArray(), itemEnchantModel);
    }

    if (value.contains("stored_enchantments")) {
        tableFromJson(value["stored_enchantments"].toArray(),
                      storedEnchantModel);
    }
}

void ItemConditionDialog::onAddedEnchant() {
    if (ui->enchant_levelInput->getMinimum() == 0
        || ui->enchant_levelInput->getMaximum() == 0) {
        return;
    }
    QString enchantmentText = ui->enchant_combo->currentText();
    if (!itemEnchantModel.findItems(enchantmentText).isEmpty())
        return;

    QStandardItem *enchantItem = new QStandardItem(enchantmentText);
    enchantItem->setData(ui->enchant_combo->currentData(Qt::UserRole + 1));
    enchantItem->setEditable(false);
    enchantItem->setToolTip(deletiveToolTip);
    QStandardItem *levelsItem = new QStandardItem();
    auto           json       = ui->enchant_levelInput->toJson();
    levelsItem->setData(json, Qt::DisplayRole);
    levelsItem->setToolTip(deletiveToolTip);
    itemEnchantModel.appendRow({ enchantItem, levelsItem });
}

void ItemConditionDialog::onAddedStoredEnchant() {
    if (ui->stored_levelInput->getMinimum() == 0
        || ui->stored_levelInput->getMaximum() == 0) {
        return;
    }
    QString enchantmentText = ui->stored_combo->currentText();
    if (!storedEnchantModel.findItems(enchantmentText).isEmpty())
        return;

    QStandardItem *enchantItem = new QStandardItem(enchantmentText);
    enchantItem->setData(ui->stored_combo->currentData(Qt::UserRole + 1));
    enchantItem->setEditable(false);
    enchantItem->setToolTip(deletiveToolTip);
    QStandardItem *levelsItem = new QStandardItem();
    auto           json       = ui->stored_levelInput->toJson();
    levelsItem->setData(json, Qt::DisplayRole);
    levelsItem->setToolTip(deletiveToolTip);
    storedEnchantModel.appendRow({ enchantItem, levelsItem });
}

void ItemConditionDialog::checkOK() {
    if (ui->itemSlot->getItems().isEmpty())
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ItemConditionDialog::initModelView(QStandardItemModel &model,
                                        QTableView *tableView) {
    QStandardItem *enchantItem = new QStandardItem("Enchantment");

    enchantItem->setToolTip("An enchantment of the tool.");
    QStandardItem *levelsItem = new QStandardItem("Levels");
    levelsItem->setToolTip("The levels of the enchantment.");

    model.setHorizontalHeaderItem(0, enchantItem);
    model.setHorizontalHeaderItem(1, levelsItem);

    tableView->setModel(&model);
    tableView->installEventFilter(&viewFilter);
    auto *delegate = new ExtendedNumericDelegate();
    delegate->setExNumInputTypes(ExtendedNumericInput::Exact
                                 | ExtendedNumericInput::Range);
    delegate->setExNumInputGeneralMin(0);
    tableView->setItemDelegate(delegate);
}

void ItemConditionDialog::tableFromJson(const QJsonArray &jsonArr,
                                        QStandardItemModel &model) {
    for (auto enchantment : jsonArr) {
        auto enchantObj = enchantment.toObject();
        if (enchantObj.isEmpty()) continue;
        if (!enchantObj.contains("enchantment")
            || !enchantObj.contains("levels")) {
            continue;
        }
        auto enchantId = enchantObj["enchantment"].toString();
        if (!enchantId.contains(":"))
            enchantId = "minecraft:" + enchantId;
        auto indexes = model.match(
            model.index(0, 0), Qt::UserRole + 1, enchantId);
        if (indexes.isEmpty()) continue;
        QStandardItem *enchantItem = new QStandardItem();
        enchantItem->setData(enchantId);
        enchantItem->setText(indexes[0].data(Qt::DisplayRole).toString());
        enchantItem->setEditable(false);
        enchantItem->setToolTip(deletiveToolTip);
        QStandardItem *levelsItem = new QStandardItem();
        levelsItem->setData(enchantObj.value("levels"), Qt::DisplayRole);
        levelsItem->setToolTip(deletiveToolTip);
        model.appendRow({ enchantItem, levelsItem });
    }
}
