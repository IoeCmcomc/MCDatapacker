#include "itemconditiondialog.h"
#include "ui_itemconditiondialog.h"

#include "numberproviderdelegate.h"
#include "inventoryslot.h"
#include "extendedtablewidget.h"

#include "game.h"
#include "platforms/windows_specific.h"

#include <QJsonObject>

ItemConditionDialog::ItemConditionDialog(QWidget *parent) :
    QDialog(parent), BaseCondition(),
    ui(new Ui::ItemConditionDialog) {
    ui->setupUi(this);

    const bool from_1_17 = Game::version() >= Game::v1_17;

    ui->itemSlot->setAcceptTag(false);
    ui->itemSlot->setAcceptMultiple(from_1_17);

    initComboModelView("potion", potionsModel, ui->potionCombo);
    initComboModelView("enchantment", enchantmentsModel, ui->enchant_combo,
                       false);
    ui->stored_combo->setModel(&enchantmentsModel);

    initTable(ui->enchantmentsTable);
    initTable(ui->storedEnchantsTable);

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

void ItemConditionDialog::initTable(ExtendedTableWidget *table) {
    table->appendColumnMapping("enchantment", ui->enchant_combo);
    table->appendColumnMapping("levels", ui->enchant_levelInput);

    auto *delegate = new NumberProviderDelegate(this);

    delegate->setInputModes(NumberProvider::ExactAndRange);
    table->tableWidget()->setItemDelegateForColumn(1, delegate);
}
