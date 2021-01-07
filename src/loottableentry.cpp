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

    ui->conditionsContainer->setLayout(&conditionsLayout);
    ui->functionsContainer->setLayout(&functionsLayout);
    ui->entriesContainer->setLayout(&entriesLayout);

    connect(ui->deleteButton, &QToolButton::clicked, this,
            &QObject::deleteLater);
    connect(ui->typeCmobo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LootTableEntry::onTypeChanged);
    connect(ui->entryGroupLabel, &QLabel::linkActivated,
            [this]() {
        setCurrentIndex(ENTRIES_TAB);
    });
    connect(ui->addCondButton, &QToolButton::clicked,
            this, &LootTableEntry::onAddCondition);
    connect(ui->addFunctButton, &QToolButton::clicked,
            this, &LootTableEntry::onAddFunction);
    connect(ui->addEntryButton, &QToolButton::clicked,
            this, &LootTableEntry::onAddEntry);

    ui->itemSlot->setAcceptMultiItems(false);
    ui->itemSlot->setAcceptTag(false);
}

LootTableEntry::~LootTableEntry() {
    delete ui;
}

void LootTableEntry::fromJson(const QJsonObject &root) {
    if (!root.contains("type"))
        return;

    QString type = root.value("type").toString();
    Glhp::removePrefix(type, "minecraft:");

    const int index = entryTypes.indexOf(type);
    if (index == -1)
        return;

    if (root.contains("quality")) {
        ui->qualitySpin->setEnabled(true);
        ui->qualitySpin->setValue(root.value("quality").toInt());
    }

    ui->typeCmobo->setCurrentIndex(index);
    switch (index) {
    case 0:     /*Empty */
        break;

    case 1: {      /*Item */
        if (root.contains("name"))
            ui->itemSlot->appendItem(MCRInvItem(root.value("name").toString()));
        break;
    }

    case 2: {     /*Loot table */
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
            QJsonArray children = root.value("children").toArray();
            Glhp::loadJsonToObjectsToLayout<LootTableEntry>(children,
                                                            entriesLayout);
        }

        break;
    }
    }

    if (root.contains("conditions")) {
        QJsonArray conditions = root.value("conditions").toArray();
        Glhp::loadJsonToObjectsToLayout<MCRPredCondition>(conditions,
                                                          conditionsLayout);
    }
    if (root.contains("functions")) {
        QJsonArray functions = root.value("functions").toArray();
        Glhp::loadJsonToObjectsToLayout<LootTableFunction>(functions,
                                                           functionsLayout);
    }
}

QJsonObject LootTableEntry::toJson() const {
    QJsonObject root;
    const int   index = ui->typeCmobo->currentIndex();

    root.insert("type", "minecraft:" + entryTypes[index]);
    if (ui->qualitySpin->isEnabled())
        root.insert("quality", ui->qualitySpin->value());
    if (ui->typeCmobo->currentIndex())
        switch (index) {
        case 0: /*Empty */
            break;

        case 1: {  /*Item */
            if (ui->itemSlot->getItems().count() == 1)
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

            auto children = Glhp::getJsonFromObjectsFromParent<
                LootTableEntry>(ui->entriesContainer);
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

    auto conditions = Glhp::getJsonFromObjectsFromParent<MCRPredCondition>(
        ui->conditionsContainer);
    if (!conditions.isEmpty())
        root.insert("conditions", conditions);

    auto functions = Glhp::getJsonFromObjectsFromParent<LootTableFunction>(
        ui->functionsContainer);
    if (!functions.isEmpty())
        root.insert("functions", functions);

    return root;
}

void LootTableEntry::onTypeChanged(int index) {
    ui->nameEdit->setPlaceholderText("namespace:id");
    setTabEnabled(ENTRIES_TAB, index == 4);
    switch (index) {
    case 0: { /*Empty */
        ui->stackedWidget->setCurrentIndex(0);
        break;
    }

    case 1:   /*Item */
    case 2: { /*Loot table */
        ui->stackedWidget->setCurrentIndex(1);
        ui->tagExpandCheck->setEnabled(false);
        break;
    }

    case 3: {/*Tag */
        ui->stackedWidget->setCurrentIndex(1);
        ui->tagExpandCheck->setEnabled(true);
        break;
    }

    case 4: {/*Group */
        ui->stackedWidget->setCurrentIndex(2);
        break;
    }

    case 5: {/*Dynamic */
        ui->stackedWidget->setCurrentIndex(1);
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

void LootTableEntry::onAddCondition() {
    auto *cond = new MCRPredCondition(ui->conditionsContainer);

    conditionsLayout.addWidget(cond, 0);
}

void LootTableEntry::onAddFunction() {
    auto *funct = new LootTableFunction(ui->functionsArea);

    functionsLayout.addWidget(funct, 0);
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
        /*Glhp::deleteChildrenIn(ui->entriesContainer); */
        break;
    }

    case 5: {    /*Dynamic */
        ui->nameEdit->clear();
        break;
    }

    default:
        break;
    }

    Glhp::deleteChildrenIn(ui->conditionsContainer);
    Glhp::deleteChildrenIn(ui->entriesContainer);
    Glhp::deleteChildrenIn(ui->functionsContainer);
}

void LootTableEntry::onAddEntry() {
    auto *entry =
        new LootTableEntry(ui->entriesContainer);

    entry->setLootTableEditor(lootTableEditor);
    entriesLayout.addWidget(entry, 0);
}

void LootTableEntry::setLootTableEditor(QWidget *value) {
    lootTableEditor = value;
}
