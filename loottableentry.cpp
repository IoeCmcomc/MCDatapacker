#include "loottableentry.h"
#include "ui_loottableentry.h"

#include "mcrpredcondition.h"

#include <QDebug>

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
            [this](const QString &link) {
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
        break;
    }

    default:
        break;
    }
}

void LootTableEntry::onAddCondition() {
    MCRPredCondition *cond = new MCRPredCondition(ui->conditionsContainer);

    conditionsLayout.addWidget(cond, 0);
}

void LootTableEntry::onAddEntry() {
    LootTableEntry *entry = new LootTableEntry(ui->entriesContainer);

    entriesLayout.addWidget(entry, 0);
}
