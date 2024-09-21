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

    ui->itemSlot->setAcceptItemsOrTag();
    if (UNTIL_VER(v1_20_6)) {
        if (UNTIL_VER(v1_17)) {
            ui->itemSlot->setAcceptMultiple(false);
        }

        ui->stackedWidget->setCurrentIndex(0);
        ui->tabWidget->tabBar()->setEnabled(false);
        ui->tabWidget->tabBar()->hide();
    }

    m_potionModel.setInfo(QStringLiteral("potion"));
    ui->potionCombo->setModel(&m_potionModel);
    m_enchantmentModel.setInfo(QStringLiteral("enchantment"),
                               GameInfoModel::PrependPrefix |
                               GameInfoModel::DontShowIcons);
    ui->enchant_combo->setModel(&m_enchantmentModel);
    ui->stored_combo->setModel(&m_enchantmentModel);

    initTable(ui->enchantmentsTable, ui->enchant_combo, ui->enchant_levelInput);
    initTable(ui->storedEnchantsTable, ui->stored_combo, ui->stored_levelInput);

    m_controller.addMappings({
        { FROM_VER(v1_17) ? "items" : "item", ui->itemSlot, false,
          FROM_VER(v1_20_6) },
        { QStringLiteral("count"), ui->countInput },
    });
    if (FROM_VER(v1_20_6)) {
        m_controller.addMapping("components", ui->componentsBtn);
    } else {
        m_controller.addMappings({
            { "durability", ui->durabilityInput },
            { "nbt", ui->NBTEdit },
            { "potion", ui->potionCombo },
            { "enchantments", ui->enchantmentsTable },
            { "stored_enchantments", ui->storedEnchantsTable },
        });
    }

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
