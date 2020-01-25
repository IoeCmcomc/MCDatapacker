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

    this->CraftingSlots = QVector<MCRInvSlot*>({ui->craftingSlot_0, ui->craftingSlot_1,
                     ui->craftingSlot_2, ui->craftingSlot_3,
                     ui->craftingSlot_4, ui->craftingSlot_5,
                     ui->craftingSlot_6, ui->craftingSlot_7,
                     ui->craftingSlot_8, ui->craftingSlot_9
                    });

    foreach(MCRInvSlot *slot, this->CraftingSlots) {
        connect(slot, SIGNAL(itemChanged()), this, SLOT(onRecipeChanged()));
    }
    connect(ui->resultCountInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->craftTypeInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VisualRecipeEditorDock::onRecipeChanged);
    connect(ui->craftGroupInput, &QLineEdit::textChanged, this, &VisualRecipeEditorDock::onRecipeChanged);

    setupItemList();

}

VisualRecipeEditorDock::~VisualRecipeEditorDock()
{
    delete ui;
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

void VisualRecipeEditorDock::onRecipeChanged() {
    int tabIdx = ui->tabWidget->currentIndex();
    if(tabIdx == 0) { // Crafting tab
        QJsonObject root;
        int craftTypeIdx = ui->craftTypeInput->currentIndex();
        if(craftTypeIdx < 2) { // Shaped crafting
            root.insert("type", "crafting_shaped");

            QMap<QString, QString> keys;
            QString patternStr;
            QString patternChars = "123456789";
            int pattCharIdx = -1;
            for(int i = 1; i < 10; ++i) {
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
            //qDebug() << keys;
            //qDebug() << patternStr;

            QJsonArray pattern;
            while(!patternStr.isEmpty()) {
                pattern.push_back(patternStr.left(3));
                patternStr.remove(0, 3);
            }

            if(craftTypeIdx == 1) { // Relative shape
                {
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

                    //qDebug() << xstart << xend << ystart << yend;

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
            for(auto str : keys.toStdMap())
            {
                QJsonObject keyItem;
                keyItem.insert("item", "minecraft:" + str.first);
                key.insert(str.second, keyItem);
            }
            root.insert("key", key);

        } else { // Shapeless crafting
            root.insert("type", "crafting_shapeless");
        }

        QJsonObject result;
        result.insert("item", (ui->craftingSlot_0->getItem())
                              ? ("minecraft:"
                                 + ui->craftingSlot_0->getItem()->namespacedID)
                              : "");
        result.insert("count", ui->resultCountInput->value());
        root.insert("result", result);

        root.insert("group", ui->craftGroupInput->text());

        QJsonDocument jsonDoc = QJsonDocument(root);
        qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
        //qDebug().noquote() << jsonDoc.toJson();
    }
}
//shrink
