#include "loottableentry.h"
#include "ui_loottableentry.h"

#include "mcrpredcondition.h"
#include "loottablefunction.h"
#include "loottableeditordock.h"
#include "mcrinvslot.h"
#include "mcrinvitem.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QJsonArray>

LootTableEntry::LootTableEntry(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTableEntry) {
    ui->setupUi(this);
    setTabEnabled(ENTRIES_TAB, false);

    connect(ui->deleteButton, &QToolButton::clicked, this,
            &QObject::deleteLater);
    connect(ui->typeCmobo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LootTableEntry::onTypeChanged);
    connect(this,
            &QTabWidget::currentChanged,
            this,
            &LootTableEntry::onTabChanged);
    connect(ui->entryGroupLabel, &QLabel::linkActivated, [this]() {
        setCurrentIndex(ENTRIES_TAB);
    });

    ui->itemSlot->setAcceptMultiItems(false);
    ui->itemSlot->setAcceptTag(false);
}

LootTableEntry::~LootTableEntry() {
    delete ui;
}

void LootTableEntry::fromJson(const QJsonObject &root) {
    resetAll();
    if (!root.contains("type"))
        return;

    QString type = root.value("type").toString();
    Glhp::removePrefix(type, "minecraft:");

    const int index = entryTypes.indexOf(type);
    if (index == -1)
        return;

    if (root.contains("quality")) {
        ui->qualitySpin->setValue(root.value("quality").toInt());
    }
    if (root.contains("weight"))
        ui->weightSpin->setValue(root.value("weight").toInt());

    ui->typeCmobo->setCurrentIndex(index);
    switch (index) {
    case 0:     /* Empty */
        break;

    case 1: {      /* Item */
        if (root.contains("name"))
            ui->itemSlot->appendItem(MCRInvItem(root.value("name").toString()));
        break;
    }

    case 2: {     /* Loot table */
        if (root.contains("name"))
            ui->nameEdit->setText(root.value("name").toString());
        break;
    }

    case 3: {    /*Tag */
        if (root.contains("name"))
            ui->nameEdit->setText(root.value("name").toString());
        ui->tagExpandCheck->setupFromJsonObject(root, "expand");
        break;
    }


    case 4: {    /*Dynamic */
        if (root.contains("name")) {
            auto name = root.value("name").toString();
            if (name == QStringLiteral("minecraft:contents")
                || name == QStringLiteral("minecraft:self")) {
                ui->nameEdit->setText(name);
            }
        }
        break;
    }

    default: {     /*Group */
        if (type == "alternatives") {
            ui->selectAltRadio->setChecked(true);
        } else if (type == "group") {
            ui->selectAllRadio->setChecked(true);
        } else if (type == "sequence") {
            ui->selectAltRadio->setChecked(true);
        } else {
            break;
        }

        if (root.contains("children")) {
            if (!ui->entriesInterface->mainWidget())
                initEntryInterface();
            ui->entriesInterface->setJson(root.value("children").toArray());
        }

        break;
    }
    }

    if (root.contains("conditions")) {
        if (!ui->conditionsInterface->mainWidget())
            initCondInterface();
        ui->conditionsInterface->setJson(root.value("conditions").toArray());
    }
    if (root.contains("functions")) {
        if (!ui->functionsInterface->mainWidget())
            initFuncInterface();
        ui->functionsInterface->setJson(root.value("functions").toArray());
    }
}

QJsonObject LootTableEntry::toJson() const {
    QJsonObject root;
    const int   index = ui->typeCmobo->currentIndex();

    root.insert("type", "minecraft:" + entryTypes[index]);
    root.insert("weight", ui->weightSpin->value());
    if (const int quality = ui->qualitySpin->value(); quality != 0)
        root.insert("quality", quality);
    if (ui->typeCmobo->currentIndex())
        switch (index) {
        case 0: /*Empty */
            break;

        case 1: {  /*Item */
            if (ui->itemSlot->size() == 1)
                root.insert("name", ui->itemSlot->itemNamespacedID(0));
            break;
        }

        case 2: { /*Loot table */
            if (!ui->nameEdit->text().isEmpty())
                root.insert("name", ui->nameEdit->text());
            break;
        }

        case 3: {/*Tag */
            if (!ui->nameEdit->text().isEmpty())
                root.insert("name", ui->nameEdit->text());
            ui->tagExpandCheck->insertToJsonObject(root, "expand");
            break;
        }

        case 4: {/*Group */
            if (ui->selectAltRadio->isChecked())
                root.insert("type", "minecraft:alternatives");
            else if (ui->selectSeqRadio->isChecked())
                root.insert("type", "minecraft:sequence");

            auto &&children = ui->entriesInterface->json();
            if (!children.isEmpty())
                root.insert("children", children);
            break;
        }

        case 5: {/*Dynamic */
            if ((ui->nameEdit->text() == QStringLiteral("minecraft:contents"))
                || (ui->nameEdit->text() == QStringLiteral("minecraft:self"))) {
                root.insert("name", ui->nameEdit->text());
            }
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
    for (int i = 0; i < ui->stackedWidget->count(); ++i)
        reset(i);
    ui->functionsInterface->setJson({});
    ui->conditionsInterface->setJson({});
}

void LootTableEntry::onTypeChanged(int index) {
    ui->nameEdit->setPlaceholderText("namespace:id");
    setTabEnabled(ENTRIES_TAB, index == 4);
    switch (index) {
    case 0: { /*Empty */
        ui->stackedWidget->setCurrentIndex(0);
        break;
    }

    case 1: {  /*Item */
        ui->stackedWidget->setCurrentIndex(1);
        break;
    }

    case 2: { /*Loot table */
        ui->stackedWidget->setCurrentIndex(2);
        ui->tagExpandCheck->setEnabled(false);
        break;
    }

    case 3: {/*Tag */
        ui->stackedWidget->setCurrentIndex(2);
        ui->tagExpandCheck->setEnabled(true);
        break;
    }

    case 4: {/*Group */
        ui->stackedWidget->setCurrentIndex(3);
        break;
    }

    case 5: {/*Dynamic */
        ui->stackedWidget->setCurrentIndex(2);
        ui->nameEdit->setPlaceholderText(tr("%1 or %2",
                                            "\"minecraft:contents or minecraft:self\"").
                                         arg(QStringLiteral("minecraft:contents"),
                                             QStringLiteral("minecraft:self")));
        ui->tagExpandCheck->setEnabled(false);
        break;
    }

    default:
        break;
    }
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

void LootTableEntry::reset(int index) {
    switch (index) {
    case 0:     /*Empty */
        break;

    case 1: {      /*Item */
        ui->itemSlot->clearItems();
        break;
    }

    case 2: {     /*Loot table */
        ui->nameEdit->clear();
        break;
    }

    case 3: {    /*Tag */
        ui->nameEdit->clear();
        ui->tagExpandCheck->unset();
        break;
    }

    case 4: {    /*Group */
        ui->entriesInterface->setJson({});
        break;
    }

    case 5: {    /*Dynamic */
        ui->nameEdit->clear();
        break;
    }

    default:
        break;
    }
}

void LootTableEntry::initCondInterface() {
    auto *cond = new MCRPredCondition();

    ui->conditionsInterface->setMainWidget(cond);
    ui->conditionsInterface->mapToSetter(
        cond, qOverload<const QJsonObject &>(&MCRPredCondition::fromJson));
    ui->conditionsInterface->mapToGetter(&MCRPredCondition::toJson, cond);
}

void LootTableEntry::initFuncInterface() {
    auto *func = new LootTableFunction();

    ui->functionsInterface->setupMainWidget(func);
}

void LootTableEntry::initEntryInterface() {
    auto *entry = new LootTableEntry();

    ui->entriesInterface->setupMainWidget(entry);
}
