#include "loottableentry.h"
#include "ui_loottableentry.h"

#include "loottablecondition.h"
#include "loottablefunction.h"
#include "inventoryslot.h"
#include "inventoryitem.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QJsonArray>

LootTableEntry::LootTableEntry(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTableEntry) {
    ui->setupUi(this);
    setTabEnabled(ENTRIES_TAB, false);

    connect(ui->multiPageWidget, &MultiPageWidget::currentIndexChanged,
            this, &LootTableEntry::onTypeChanged);
    connect(this, &QTabWidget::currentChanged,
            this, &LootTableEntry::onTabChanged);
    connect(ui->entryGroupLabel, &QLabel::linkActivated, this, [this]() {
        setCurrentIndex(ENTRIES_TAB);
    });
    connect(ui->entriesInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableEntry::updateEntriesTab);
    connect(ui->functionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableEntry::updateFunctionsTab);
    connect(ui->conditionsInterface, &DataWidgetInterface::entriesCountChanged,
            this, &LootTableEntry::updateConditionsTab);

    ui->itemSlot->setAcceptMultiple(false);
    ui->itemSlot->setAcceptTag(false);

    updateEntriesTab(0);
}

LootTableEntry::~LootTableEntry() {
    delete ui;
}

void LootTableEntry::fromJson(const QJsonObject &root) {
    resetAll();
    if (!root.contains("type"))
        return;

    QString type = root.value(QLatin1String("type")).toString();
    Glhp::removePrefix(type, "minecraft:"_QL1);

    const int index = entryTypes.indexOf(type);
    if (index == -1)
        return;

    if (root.contains(QLatin1String("quality"))) {
        ui->qualitySpin->setValue(root.value(QLatin1String("quality")).toInt());
    }
    if (root.contains(QLatin1String("weight")))
        ui->weightSpin->setValue(root.value(QLatin1String("weight")).toInt());

    ui->multiPageWidget->setCurrentIndex(index);
    switch (index) {
        case 0: /* Empty */
            break;

        case 1: {  /* Item */
            if (root.contains(QLatin1String("name")))
                ui->itemSlot->setItem(InventoryItem(root.value(QLatin1String(
                                                                   "name"))
                                                    .toString()));
            break;
        }

        case 2: { /* Loot table */
            if (root.contains(QLatin1String("name")))
                ui->tableNameEdit->setText(root.value(QLatin1String(
                                                          "name")).toString());
            break;
        }

        case 3: { /*Tag */
            if (root.contains(QLatin1String("name")))
                ui->tagNameEdit->setText(root.value(QLatin1String(
                                                        "name")).toString());
            ui->tagExpandCheck->setupFromJsonObject(root,
                                                    QStringLiteral("expand"));
            break;
        }


        case 4: { /* Dynamic */
            if (root.contains(QLatin1String("name"))) {
                auto name = root.value(QLatin1String("name")).toString();
                if (name == QLatin1String("minecraft:contents")
                    || name == QLatin1String("minecraft:self")) {
                    ui->dynamicNameEdit->setText(name);
                }
            }
            break;
        }

        default: { /* Group */
            if (type == QLatin1String("alternatives")) {
                ui->selectAltRadio->setChecked(true);
            } else if (type == QLatin1String("group")) {
                ui->selectAllRadio->setChecked(true);
            } else if (type == QLatin1String("sequence")) {
                ui->selectAltRadio->setChecked(true);
            } else {
                break;
            }

            if (root.contains(QLatin1String("children"))) {
                if (!ui->entriesInterface->mainWidget())
                    initEntryInterface();
                ui->entriesInterface->setJson(
                    root.value(QLatin1String("children")).toArray());
            }
        }
    }

    if (root.contains(QLatin1String("conditions"))) {
        if (!ui->conditionsInterface->mainWidget())
            initCondInterface();
        ui->conditionsInterface->setJson(
            root.value(QLatin1String("conditions")).toArray());
    }
    if (root.contains(QLatin1String("functions"))) {
        if (!ui->functionsInterface->mainWidget())
            initFuncInterface();
        ui->functionsInterface->setJson(
            root.value(QLatin1String("functions")).toArray());
    }
}

QJsonObject LootTableEntry::toJson() const {
    QJsonObject root;
    const int   index = ui->multiPageWidget->currentIndex();

    const QString &&type = QStringLiteral("minecraft:") + entryTypes[index];

    root.insert("type", type);
    root.insert("weight", ui->weightSpin->value());
    if (const int quality = ui->qualitySpin->value(); quality != 0)
        root.insert("quality", quality);
    if (ui->multiPageWidget->currentIndex())
        switch (index) {
            case 0: /*Empty */
                break;

            case 1: { /*Item */
                if (ui->itemSlot->itemCount() == 1)
                    root.insert("name", ui->itemSlot->itemNamespacedID(0));
                break;
            }

            case 2: { /*Loot table */
                if (!ui->tableNameEdit->text().isEmpty())
                    root.insert("name", ui->tableNameEdit->text());
                break;
            }

            case 3: {/*Tag */
                if (!ui->tagNameEdit->text().isEmpty())
                    root.insert("name", ui->tagNameEdit->text());
                ui->tagExpandCheck->insertToJsonObject(root, "expand");
                break;
            }

            case 4: {/* Dynamic */
                if ((ui->dynamicNameEdit->text() ==
                     QLatin1String("minecraft:contents"))
                    || (ui->dynamicNameEdit->text() ==
                        QLatin1String("minecraft:self"))) {
                    root.insert("name", ui->dynamicNameEdit->text());
                }
                break;
            }

            case 5: {/* Group */
                if (ui->selectAltRadio->isChecked())
                    root.insert("type", "minecraft:alternatives");
                else if (ui->selectSeqRadio->isChecked())
                    root.insert("type", "minecraft:sequence");

                auto &&children = ui->entriesInterface->json();
                if (!children.isEmpty())
                    root.insert("children", children);
                break;
            }

            default:
                break;
        }

    auto &&conditions = ui->conditionsInterface->json();
    if (!conditions.isEmpty())
        root.insert("conditions", conditions);

    auto &&functions = ui->functionsInterface->json();
    if (!functions.isEmpty())
        root.insert("functions", functions);

    return root;
}

void LootTableEntry::resetAll() {
    ui->qualitySpin->setValue(0);
    ui->weightSpin->setValue(0);
    ui->functionsInterface->setJson({});
    ui->conditionsInterface->setJson({});
}

void LootTableEntry::changeEvent(QEvent *event) {
    QTabWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateEntriesTab(ui->entriesInterface->entriesCount());
        updateFunctionsTab(ui->functionsInterface->entriesCount());
        updateConditionsTab(ui->conditionsInterface->entriesCount());
    }
}

void LootTableEntry::onTypeChanged(int index) {
    setTabEnabled(ENTRIES_TAB, index == 5);
}

void LootTableEntry::onTabChanged(int index) {
    switch (index) {
        case ENTRIES_TAB: {
            if (!ui->entriesInterface->mainWidget())
                initEntryInterface();
            break;
        }

        case 2: { /* Functions */
            if (!ui->functionsInterface->mainWidget())
                initFuncInterface();
            break;
        }

        case 3: { /* Conditions */
            if (!ui->conditionsInterface->mainWidget())
                initCondInterface();
            break;
        }

        default:
            break;
    }
}

void LootTableEntry::updateEntriesTab(int size) {
    setTabText(1, tr("Entries (%1)").arg(size));
}

void LootTableEntry::updateFunctionsTab(int size) {
    setTabText(2, tr("Functions (%1)").arg(size));
}

void LootTableEntry::updateConditionsTab(int size) {
    setTabText(3, tr("Conditions (%1)").arg(size));
}

void LootTableEntry::initCondInterface() {
    auto *cond = new LootTableCondition();

    ui->conditionsInterface->setMainWidget(cond);
    ui->conditionsInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&LootTableCondition::fromJson));
    ui->conditionsInterface->mapToGetter(&LootTableCondition::toJson, cond);
}

void LootTableEntry::initFuncInterface() {
    auto *func = new LootTableFunction();

    ui->functionsInterface->setupMainWidget(func);
}

void LootTableEntry::initEntryInterface() {
    auto *entry = new LootTableEntry();

    ui->entriesInterface->setupMainWidget(entry);
}
