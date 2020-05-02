#include "loottableentry.h"
#include "ui_loottableentry.h"

#include "mcrpredcondition.h"
#include "loottableeditordock.h"

#include <QDebug>
#include <QJsonArray>

LootTableEntry::LootTableEntry(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::LootTableEntry) {
    ui->setupUi(this);
    setTabEnabled(ENTRIES_TAB, false);

    ui->conditionsContainer->setLayout(&conditionsLayout);
    ui->entriesContainer->setLayout(&entriesLayout);

    connect(ui->deleteButton, &QPushButton::clicked, this,
            &QObject::deleteLater);
    connect(ui->typeCmobo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LootTableEntry::onTypeChanged);
    connect(ui->entryGroupLabel, &QLabel::linkActivated,
            [this]() {
        setCurrentIndex(ENTRIES_TAB);
    });
    connect(ui->addCondButton, &QPushButton::clicked,
            this, &LootTableEntry::onAddCondition);
    connect(ui->addEntryButton, &QPushButton::clicked,
            this, &LootTableEntry::onAddEntry);
}

LootTableEntry::~LootTableEntry() {
    delete ui;
}

void LootTableEntry::fromJson(const QJsonObject &root) {
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

        case 1:   /*Item */
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
            int childCount = ui->entriesContainer->children().count();
            if (childCount != 0) {
                QJsonArray children;
                for (auto *child : ui->entriesContainer->children()) {
                    LootTableEntry *childEntry = qobject_cast<LootTableEntry*>(
                        child);
                    if (childEntry != nullptr)
                        children.push_back(childEntry->toJson());
                }
                root.insert("children", children);
            }

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

    int childCount = ui->conditionsContainer->children().count();
    if (childCount != 0) {
        QJsonArray conditions;
        for (auto *child : ui->conditionsContainer->children()) {
            MCRPredCondition *childCond = qobject_cast<MCRPredCondition*>(
                child);
            if (childCond != nullptr)
                conditions.push_back(childCond->toJson());
        }
        root.insert("conditions", conditions);
    }

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

void LootTableEntry::onLuckBasedChanged(bool checked) {
    setIsLuckBased(checked);
}

void LootTableEntry::onAddCondition() {
    MCRPredCondition *cond = new MCRPredCondition(ui->conditionsContainer);

    conditionsLayout.addWidget(cond, 0);
}

void LootTableEntry::onAddEntry() {
    LootTableEntry *entry =
        new LootTableEntry(ui->entriesContainer);

    entry->setLootTableEditor(lootTableEditor);
    entriesLayout.addWidget(entry, 0);
}

void LootTableEntry::setLootTableEditor(QWidget *value) {
    lootTableEditor = value;
    connect(qobject_cast<LootTableEditorDock*>(lootTableEditor),
            &LootTableEditorDock::isLuckBasedChanged,
            this, &LootTableEntry::onLuckBasedChanged);
}

void LootTableEntry::setIsLuckBased(bool value) {
    isLuckBased = value;
    ui->qualityLabel->setEnabled(isLuckBased);
    ui->qualitySpin->setEnabled(isLuckBased);
}
