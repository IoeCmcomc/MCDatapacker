#include "visualrecipeeditordock.h"
#include "ui_visualrecipeeditordock.h"

#include "mainwindow.h"

#include "mcrinvslot.h"
#include "mcrinvitem.h"

#include <QDebug>
#include <QMap>
#include <QStringLiteral>
#include <QLabel>
#include <QListWidgetItem>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>


VisualRecipeEditorDock::VisualRecipeEditorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VisualRecipeEditorDock)
{
    ui->setupUi(this);

    connect(ui->outputSlot, &MCRInvSlot::itemChanged, this, &VisualRecipeEditorDock::onRecipeChanged);
    this->CraftingSlots = QVector<MCRInvSlot*>({ui->craftingSlot_1,
                     ui->craftingSlot_2, ui->craftingSlot_3,
                     ui->craftingSlot_4, ui->craftingSlot_5,
                     ui->craftingSlot_6, ui->craftingSlot_7,
                     ui->craftingSlot_8, ui->craftingSlot_9
                    });

    foreach(MCRInvSlot *slot, this->CraftingSlots) {
        connect(slot, SIGNAL(itemChanged()), this, SLOT(onRecipeChanged()));
    }
    connect(ui->smeltingSlot_0, &MCRInvSlot::itemChanged, this, &VisualRecipeEditorDock::onRecipeChanged);

    connect(ui->customTabBar, &QTabBar::currentChanged, this, &VisualRecipeEditorDock::onRecipeTabChanged);
    connect(ui->resultCountInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->craftTypeInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->smeltBlockInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->experienceInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->cookTimeCheck, &QCheckBox::stateChanged, this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->cookTimeCheck, &QCheckBox::stateChanged, [this](int i){ ui->cookTimeInput->setDisabled(2-i); });
    connect(ui->cookTimeInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->recipeGroupInput, &QLineEdit::textChanged, this, &VisualRecipeEditorDock::onRecipeChanged);

    setupTabBar();
    setupItemList();
}

VisualRecipeEditorDock::~VisualRecipeEditorDock()
{
    delete ui;
}

void VisualRecipeEditorDock::setupTabBar() {
    ui->customTabBar->addTab(tr("Crafting"));
    ui->customTabBar->addTab(tr("Smelting"));
    ui->customTabBar->addTab(tr("Stonecutter"));

    ui->customTabWidgetLayout->removeWidget(ui->customTabBar);
    ui->customTabWidgetLayout->removeWidget(ui->customTabFrame);
    ui->customTabWidgetLayout->addWidget(ui->customTabBar, 0, 0, Qt::AlignTop);
    ui->customTabWidgetLayout->addWidget(ui->customTabFrame, 0, 0, 0);
    ui->customTabBar->raise();
    ui->customTabBar->setExpanding(false);
    //ui->customTabBar->setMovable(true);
}

void VisualRecipeEditorDock::setupItemList() {

    MCRInvSlot *invSlot = new MCRInvSlot(this, new MCRInvItem(this, "test_spawn_egg"));
    invSlot->isCreative = true;
    QListWidgetItem *litem = new QListWidgetItem(ui->itemList);
    litem->setSizeHint(invSlot->sizeHint());
    ui->itemList->addItem(litem);
    ui->itemList->setItemWidget(litem, invSlot);

    for(auto key : MCRInvItem::itemList().keys())
    {
        MCRInvSlot *invSlot = new MCRInvSlot(this, new MCRInvItem(this, key));
        invSlot->isCreative = true;
        connect(invSlot, SIGNAL(itemChanged()), this, SLOT(onRecipeChanged()));
        QListWidgetItem *litem = new QListWidgetItem(ui->itemList);
        litem->setSizeHint(invSlot->sizeHint());
        ui->itemList->addItem(litem);
        ui->itemList->setItemWidget(litem, invSlot);
    }
}

void VisualRecipeEditorDock::onRecipeTabChanged(int index) {
    //qDebug() << "onRecipeTabChanged";
    if(index == 0) {
        ui->stackedRecipeWidget->setCurrentIndex(0);
        ui->stackedOptions->setCurrentIndex(0);
    } else if(index == 1) {
        ui->stackedRecipeWidget->setCurrentIndex(1);
        ui->stackedOptions->setCurrentIndex(1);
    } else if(index == 2) {
        ui->stackedRecipeWidget->setCurrentIndex(2);
        ui->stackedOptions->setCurrentIndex(2);
    }

    onRecipeChanged();
}

void VisualRecipeEditorDock::onRecipeChanged() {
    int index = ui->customTabBar->currentIndex();
    //qDebug() << "Tab index:" << tabIdx;
    QJsonObject root;
    root.insert("group", ui->recipeGroupInput->text());
    QJsonDocument jsonDoc;
    if(index == 0) { // Crafting tab
        jsonDoc = QJsonDocument(genCraftingJson(root));
        //qDebug().noquote() << jsonDoc.toJson();
    } else if(index == 1) {
        jsonDoc = QJsonDocument(genSmeltingJson(root));
    } else if(index == 2) {
        jsonDoc = QJsonDocument(genStonecuttingJson(root));
    }

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}

QJsonObject VisualRecipeEditorDock::genCraftingJson(QJsonObject root) {
    int craftTypeIdx = ui->craftTypeInput->currentIndex();
    if(craftTypeIdx < 2) { // Shaped crafting
        root.insert("type", "minecraft:crafting_shaped");

        QMap<QString, QString> keys;
        QString patternStr;
        QString patternChars = "123456789";
        int pattCharIdx = -1;
        for(int i = 0; i < 9; ++i) {
            MCRInvItem *item = this->CraftingSlots[i]->getItem();
            if(!!item) {
                if(!keys.contains(item->namespacedID)) {
                    ++pattCharIdx;
                    keys[item->namespacedID] = patternChars[pattCharIdx];
                }
                patternStr += keys[item->namespacedID];
                //patternStr += patternChars[pattCharIdx];
            } else {
                patternStr += " ";
            }
        }

        QJsonArray pattern;
        while(!patternStr.isEmpty()) {
            pattern.push_back(patternStr.left(3));
            patternStr.remove(0, 3);
        }

        if(craftTypeIdx == 1) { // Relative shape
            { //shrink
                int width = 0;
                int xstart = 3;
                int xend = 0;
                int ystart = 3;
                int yend = 0;
                for(auto row : pattern) {
                    width = qMax(row.toString().length(), width);
                }
                for(int y = 0; y < width; ++y) {
                    for(int x = 0; x < pattern.count(); ++x) {
                        QChar c = pattern[y].toString()[x];
                        if(c != ' ') {
                            xstart = qMin(x, xstart);
                            xend = qMax(x, xend);
                            ystart = qMin(y, ystart);
                            yend = qMax(y, yend);
                        }
                    }
                }
                QJsonArray tmpPattern;
                for(int y = 0; y < 3; ++y) {
                    if(y >= ystart && y <= yend) {
                        tmpPattern.push_back(pattern[y]);
                    }
                }
                pattern = tmpPattern;
                for(auto row : pattern) {
                    row = row.toString().mid(xstart, xend - xstart + 1);
                }
            }
        }
        root.insert("pattern", pattern);

        QJsonObject key;
        for(auto str : keys.toStdMap()) {
            QJsonObject keyItem;
            keyItem.insert("item", "minecraft:" + str.first);
            key.insert(str.second, keyItem);
        }
        root.insert("key", key);

    } else { // Shapeless crafting
        root.insert("type", "minecraft:crafting_shapeless");

        QJsonArray ingredients;
        for(int i = 0; i < 9; ++i) {
            MCRInvItem *MCRitem = this->CraftingSlots[i]->getItem();
            if(!!MCRitem) {
                QJsonObject item;
                item.insert("item", MCRitem->namespacedID);
                ingredients.push_back(item);
            }
        }
        root.insert("ingredients", ingredients);
    }

    QJsonObject result;
    result.insert("item", ui->outputSlot->getCurrentID());
    result.insert("count", ui->resultCountInput->value());
    root.insert("result", result);

    return root;
}

QJsonObject VisualRecipeEditorDock::genSmeltingJson(QJsonObject root) {
    int index = ui->smeltBlockInput->currentIndex();
    const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"), QStringLiteral("campfire_cooking")
    };
    root.insert("type", smeltingTypes[index]);

    QJsonObject ingredient;
    ingredient.insert("item", ui->smeltingSlot_0->getCurrentID());
    root.insert("ingredient", ingredient);

    root.insert("result", ui->outputSlot->getCurrentID());
    root.insert("experience", ui->experienceInput->value());
    if(ui->cookTimeCheck->isChecked())
        root.insert("cookingtime", ui->cookTimeInput->value());
    return root;
}

QJsonObject VisualRecipeEditorDock::genStonecuttingJson(QJsonObject root) {
    QJsonObject ingredient;
    ingredient.insert("item", ui->stonecuttingSlot->getCurrentID());
    root.insert("ingredient", ingredient);

    root.insert("result", ui->outputSlot->getCurrentID());
    root.insert("count", ui->resultCountInput->value());

    return root;
}
