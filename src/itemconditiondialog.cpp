#include "itemconditiondialog.h"
#include "ui_itemconditiondialog.h"

#include "numberproviderdelegate.h"
#include "extendedtablewidget.h"

#include "game.h"

#include <QJsonObject>
#include <QTableWidget>

ItemConditionDialog::ItemConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::ItemConditionDialog) {
    ui->setupUi(this);

    const bool from_1_17 = Game::version() >= Game::v1_17;

    ui->itemSlot->setAcceptTag(false);
    ui->itemSlot->setAcceptMultiple(from_1_17);

    m_potionModel.setInfo(QStringLiteral("potion"));
    ui->potionCombo->setModel(&m_potionModel);
    m_enchantmentModel.setInfo(QStringLiteral("enchantment"),
                               GameInfoModel::PrependPrefix |
                               GameInfoModel::DontShowIcons);
    ui->enchant_combo->setModel(&m_enchantmentModel);
    ui->stored_combo->setModel(&m_enchantmentModel);

    initTable(ui->enchantmentsTable, ui->enchant_combo, ui->enchant_levelInput);
    initTable(ui->storedEnchantsTable, ui->stored_combo, ui->stored_levelInput);

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
    m_controller.addMapping(QStringLiteral("enchantments"),
                            ui->enchantmentsTable);
    m_controller.addMapping(QStringLiteral("stored_enchantments"),
                            ui->storedEnchantsTable);

    adjustSize();
}

ItemConditionDialog::~ItemConditionDialog() {
    delete ui;
}

QJsonObject ItemConditionDialog::toJson() const {
    QJsonObject root;

    m_controller.putValueTo(root, {});

    return root;
}

void ItemConditionDialog::fromJson(const QJsonObject &value) {
    if (value.isEmpty())
        return;

    m_controller.setValueFrom(value, {});
}

void ItemConditionDialog::initTable(ExtendedTableWidget *table,
                                    QComboBox *combo, NumberProvider *input) {
    table->appendColumnMapping("enchantment", combo);
    table->appendColumnMapping("levels", input);

    auto *delegate = new NumberProviderDelegate(this);

    delegate->setInputModes(NumberProvider::ExactAndRange);
    table->tableWidget()->setItemDelegateForColumn(1, delegate);
}
