#include "visualrecipeeditordock.h"
#include "ui_visualrecipeeditordock.h"

#include "mcrinvitem.h"

#include "mainwindow.h"

#include <QDebug>
#include <QMap>
#include <QStringLiteral>
#include <QLabel>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMouseEvent>

VisualRecipeEditorDock::VisualRecipeEditorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VisualRecipeEditorDock) {
    ui->setupUi(this);

    ui->outputSlot->setAcceptTag(false);

    this->craftingSlots = QVector<MCRInvSlot*>({ ui->craftingSlot_1,
                                                 ui->craftingSlot_2,
                                                 ui->craftingSlot_3,
                                                 ui->craftingSlot_4,
                                                 ui->craftingSlot_5,
                                                 ui->craftingSlot_6,
                                                 ui->craftingSlot_7,
                                                 ui->craftingSlot_8,
                                                 ui->craftingSlot_9
                                               });

    connect(ui->customTabBar,
            &QTabBar::currentChanged,
            this,
            &VisualRecipeEditorDock::onRecipeTabChanged);
    connect(ui->cookTimeCheck, &QCheckBox::stateChanged, [this](int i) {
        ui->cookTimeInput->setDisabled(2 - i);
    });
    connect(ui->writeRecipeBtn,
            &QPushButton::clicked,
            this,
            &VisualRecipeEditorDock::writeRecipe);
    connect(ui->readRecipeBtn,
            &QPushButton::clicked,
            this,
            &VisualRecipeEditorDock::readRecipe);
    connect(this, &QDockWidget::topLevelChanged, [ = ](bool floating) {
        adjustSize();
    });

    setupCustomTab();
}

VisualRecipeEditorDock::~VisualRecipeEditorDock() {
    delete ui;
}

void VisualRecipeEditorDock::setupCustomTab() {
    /*Add tabs */
    ui->customTabBar->addTab(tr("Crafting"));
    ui->customTabBar->addTab(tr("Smelting"));
    ui->customTabBar->addTab(tr("Stonecutting"));

    /*Make tab bar overlay top pixel of tab frame */
    ui->customTabWidgetLayout->removeWidget(ui->customTabBar);
    ui->customTabWidgetLayout->removeWidget(ui->customTabFrame);
    ui->customTabWidgetLayout->addWidget(ui->customTabBar, 0, 0, Qt::AlignTop);
    ui->customTabWidgetLayout->addWidget(ui->customTabFrame, 0, 0, 0);
    ui->customTabBar->raise();
    ui->customTabBar->setExpanding(false);
    /*ui->customTabBar->setMovable(true); */
}

void VisualRecipeEditorDock::retranslate() {
    ui->retranslateUi(this);

    ui->customTabBar->setTabText(0, tr("Crafting"));
    ui->customTabBar->setTabText(1, tr("Smelting"));
    ui->customTabBar->setTabText(2, tr("Stonecutting"));
}

void VisualRecipeEditorDock::onRecipeTabChanged(int index) {
    Q_ASSERT(index >= 0 && index < 3);
    ui->stackedRecipeWidget->setCurrentIndex(index);
    ui->stackedOptions->setCurrentIndex(index);
}

void VisualRecipeEditorDock::writeRecipe() {
    int         index = ui->customTabBar->currentIndex();
    QJsonObject root;

    root.insert("group", ui->recipeGroupInput->text());
    QJsonDocument jsonDoc;
    if (index == 0) {        /* Crafting tab */
        jsonDoc = QJsonDocument(genCraftingJson(root));
    } else if (index == 1) { /* Smelting tab */
        jsonDoc = QJsonDocument(genSmeltingJson(root));
    } else if (index == 2) { /* Stonecutting tab */
        jsonDoc = QJsonDocument(genStonecuttingJson(root));
    }

    qobject_cast<MainWindow*>(parent())->setCodeEditorText(jsonDoc.toJson());
}

QJsonObject VisualRecipeEditorDock::genCraftingJson(QJsonObject root) {
    int craftTypeIdx = ui->craftTypeInput->currentIndex();

    if (craftTypeIdx < 2) { /* Shaped crafting */
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:crafting_shaped"));

        QMap<QString, QString> keys;
        QString                patternStr;
        QString                patternChars = "123456789";
        int                    pattCharIdx  = -1;
        for (int i = 0; i < 9; ++i) {
            MCRInvItem item = this->craftingSlots[i]->getItem();
            if (!item.isEmpty()) {
                if (!keys.contains(item.getNamespacedID())) {
                    ++pattCharIdx;
                    keys[item.getNamespacedID()] = patternChars[pattCharIdx];
                }
                patternStr += keys[item.getNamespacedID()];
            } else {
                patternStr += QStringLiteral(" ");
            }
        }

        QJsonArray pattern;
        while (!patternStr.isEmpty()) {
            pattern.push_back(patternStr.left(3));
            patternStr.remove(0, 3);
        }

        if (craftTypeIdx == 1) { /* Relative shape */
            {                    /*shrink */
                int width  = 0;
                int xstart = 3;
                int xend   = 0;
                int ystart = 3;
                int yend   = 0;
                for (auto row : pattern) {
                    width = qMax(row.toString().length(), width);
                }
                for (int y = 0; y < width; ++y) {
                    for (int x = 0; x < pattern.count(); ++x) {
                        QChar c = pattern[y].toString()[x];
                        if (c != ' ') {
                            xstart = qMin(x, xstart);
                            xend   = qMax(x, xend);
                            ystart = qMin(y, ystart);
                            yend   = qMax(y, yend);
                        }
                    }
                }
                QJsonArray tmpPattern;
                for (int y = 0; y < 3; ++y) {
                    if (y >= ystart && y <= yend) {
                        tmpPattern.push_back(pattern[y]);
                    }
                }
                pattern = tmpPattern;
                for (auto row : pattern) {
                    row = row.toString().mid(xstart, xend - xstart + 1);
                }
            }
        }
        root.insert("pattern", pattern);

        QJsonObject key;
        for (auto str : keys.toStdMap()) {
            auto itemID = str.first;
            if (!itemID.contains(':'))
                itemID = QStringLiteral("minecraft:") + itemID;
            QJsonObject keyItem;
            if (MCRInvItem(itemID).getIsTag())
                keyItem.insert(QStringLiteral("tag"), itemID.remove(0, 1));
            else
                keyItem.insert(QStringLiteral("item"), itemID);
            key.insert(str.second, keyItem);
        }
        root.insert(QStringLiteral("key"), key);
    } else { /* Shapeless crafting */
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:crafting_shapeless"));

        QJsonArray ingredients;
        for (int i = 0; i < 9; ++i) {
            MCRInvItem MCRitem = this->craftingSlots[i]->getItem();
            if (!MCRitem.isEmpty()) {
                QJsonObject item;
                item.insert(QStringLiteral("item"), MCRitem.getNamespacedID());
                ingredients.push_back(item);
            }
        }
        root.insert(QStringLiteral("ingredients"), ingredients);
    }

    QJsonObject result;
    result.insert(QStringLiteral("item"), ui->outputSlot->itemNamespacedID());
    result.insert(QStringLiteral("count"), ui->resultCountInput->value());
    root.insert(QStringLiteral("result"), result);

    return root;
}

QJsonObject VisualRecipeEditorDock::genSmeltingJson(QJsonObject root) {
    int index = ui->smeltBlockInput->currentIndex();

    const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"),  QStringLiteral("campfire_cooking")
    };

    root.insert(QStringLiteral("type"),
                QStringLiteral("minecraft:") + smeltingTypes[index]);

    auto itemID = ui->smeltingSlot_0->itemNamespacedID();
    if (!itemID.contains(':'))
        itemID = QStringLiteral("minecraft:") + itemID;
    QJsonObject ingredient;
    if (MCRInvItem(itemID).getIsTag())
        ingredient.insert(QStringLiteral("tag"), itemID.remove(0, 1));
    else
        ingredient.insert(QStringLiteral("item"), itemID);
    root.insert("ingredient", ingredient);

    root.insert(QStringLiteral("result"), ui->outputSlot->itemNamespacedID());
    root.insert(QStringLiteral("experience"), ui->experienceInput->value());
    if (ui->cookTimeCheck->isChecked())
        root.insert(QStringLiteral("cookingtime"), ui->cookTimeInput->value());
    return root;
}

QJsonObject VisualRecipeEditorDock::genStonecuttingJson(QJsonObject root) {
    root.insert(QStringLiteral("type"),
                QStringLiteral("minecraft:stonecutting"));

    auto itemID = ui->stonecuttingSlot->itemNamespacedID();
    if (!itemID.contains(':'))
        itemID = QStringLiteral("minecraft:") + itemID;
    QJsonObject ingredient;
    if (MCRInvItem(itemID).getIsTag())
        ingredient.insert(QStringLiteral("tag"), itemID.remove(0, 1));
    else
        ingredient.insert(QStringLiteral("item"), itemID);
    root.insert(QStringLiteral("ingredient"), ingredient);

    root.insert(QStringLiteral("result"), ui->outputSlot->itemNamespacedID());
    root.insert(QStringLiteral("count"), ui->resultCountInput->value());

    return root;
}

void VisualRecipeEditorDock::readRecipe() {
    QString input = qobject_cast<MainWindow*>(parent())->getCodeEditorText();

    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    QJsonObject root = json_doc.object();
    if (root.isEmpty())
        return;

    if (root.contains(QStringLiteral("group")))
        ui->recipeGroupInput->setText(root[QStringLiteral("group")].toString());

    const QString craftingTypes[] = {
        QStringLiteral("crafting_shaped"), QStringLiteral("crafting_shapeless")
    };

    const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"),  QStringLiteral("campfire_cooking")
    };

    if (!root.contains(QStringLiteral("type"))) return;

    QString type = root[QStringLiteral("type")].toString();

    for (const QString &craftingType : craftingTypes) {
        if ((type == craftingType)
            || (type == QStringLiteral("minecraft:") + craftingType)) {
            ui->customTabBar->setCurrentIndex(0);
            readCraftingJson(root);
            return;
        }
    }
    for (const QString &smeltingType : smeltingTypes) {
        if ((type == smeltingType)
            || (type == QStringLiteral("minecraft:") + smeltingType)) {
            ui->customTabBar->setCurrentIndex(1);
            readSmeltingJson(root);
            return;
        }
    }
    if ((type != QStringLiteral("stonecutting"))
        && (type != QStringLiteral("minecraft:stonecutting"))) {
        return;
    } else {
        ui->customTabBar->setCurrentIndex(2);
        readStonecuttingJson(root);
    }
}

void VisualRecipeEditorDock::readCraftingJson(const QJsonObject &root) {
    QString type = root[QStringLiteral("type")].toString();

    if (type.endsWith(QStringLiteral("crafting_shaped"))) {
        if (!root.contains(QStringLiteral("pattern"))) return;

        QJsonArray pattern = root[QStringLiteral("pattern")].toArray();

        if (pattern.count() > 3) return;

        int rowlen = pattern[0].toString().length();
        for (auto row : pattern)
            if (row.toString().length() != rowlen) return;

        if (pattern.count() == 3 && rowlen == 3)
            ui->craftTypeInput->setCurrentIndex(0);
        else
            ui->craftTypeInput->setCurrentIndex(1);

        QString patternStr;
        for (auto row : pattern)
            patternStr += row.toString().leftJustified(3, ' ');
        patternStr = patternStr.leftJustified(9, ' ');

        if (!root.contains(QStringLiteral("key"))) return;

        QJsonObject keys = root[QStringLiteral("key")].toObject();

        for (int i = 0; i < 9; ++i) {
            QString key = QString(patternStr[i]);
            if (key == ' ') {
                craftingSlots[i]->removeItem();
            } else {
                if (!keys.contains(key)) continue;
                QJsonObject keyObj = keys[key].toObject();

                QString itemID;
                if (keyObj.contains(QStringLiteral("item"))) {
                    itemID = keyObj[QStringLiteral("item")].toString();
                } else if (keyObj.contains(QStringLiteral("tag"))) {
                    itemID = '#' + keyObj[QStringLiteral("tag")].toString();
                } else {
                    return;
                }

                if (!itemID.isEmpty()) {
                    craftingSlots[i]->setItem(MCRInvItem(itemID));
                } else {
                    craftingSlots[i]->removeItem();
                }
            }
        }
    } else if (type.endsWith(QStringLiteral("crafting_shapeless"))) {
        if (!root.contains(QStringLiteral("ingredients"))) return;

        QJsonArray ingredients = root[QStringLiteral("ingredients")].toArray();
        if (ingredients.count() > 9 || ingredients.count() < 1) return;

        for (int i = 0; i < 9; ++i) {
            if (i >= ingredients.count()) {
                craftingSlots[i]->removeItem();
            } else {
                QJsonObject ingredient = ingredients[i].toObject();
                /*qDebug() << i << ingredient; */
                QString itemID;
                if (ingredient.contains(QStringLiteral("item"))) {
                    itemID = ingredient[QStringLiteral("item")].toString();
                } else if (ingredient.contains(QStringLiteral("tag"))) {
                    itemID = '#' + ingredient[QStringLiteral("tag")].toString();
                } else {
                    return;
                }

                if (!itemID.isEmpty()) {
                    craftingSlots[i]->setItem(MCRInvItem(itemID));
                } else {
                    craftingSlots[i]->removeItem();
                }
            }
        }
    }

    if (!root.contains(QStringLiteral("result"))) return;

    QJsonObject result = root[QStringLiteral("result")].toObject();
    if (!result.contains(QStringLiteral("item"))) return;

    QString itemID = result[QStringLiteral("item")].toString();
    if (!itemID.isEmpty())
        ui->outputSlot->setItem(MCRInvItem(itemID));
    else
        ui->outputSlot->removeItem();
    if (result.contains(QStringLiteral("count")))
        ui->resultCountInput->setValue(result[QStringLiteral("count")].toInt());
}

void VisualRecipeEditorDock::readSmeltingJson(const QJsonObject &root) {
    QString       type            = root[QStringLiteral("type")].toString();
    const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"),  QStringLiteral("campfire_cooking")
    };

    /*qDebug() << smeltingTypes->indexOf(type); */
    for (int i = 0; i < smeltingTypes->length(); ++i) {
        if (type.endsWith(smeltingTypes[i])) {
            ui->smeltBlockInput->setCurrentIndex(i);
            break;
        }
    }

    if (!root.contains(QStringLiteral("ingredient"))) return;

    QJsonObject ingredient = root[QStringLiteral("ingredient")].toObject();
    QString     itemID;
    if (ingredient.contains(QStringLiteral("item"))) {
        itemID = ingredient[QStringLiteral("item")].toString();
    } else if (ingredient.contains(QStringLiteral("tag"))) {
        itemID = '#' + ingredient[QStringLiteral("tag")].toString();
    } else {
        return;
    }

    if (!itemID.isEmpty())
        ui->smeltingSlot_0->setItem(MCRInvItem(itemID));
    else
        ui->smeltingSlot_0->removeItem();
    if (!root.contains(QStringLiteral("result"))) return;

    itemID = root[QStringLiteral("result")].toString();
    if (!itemID.isEmpty())
        ui->outputSlot->setItem(MCRInvItem(itemID));
    else
        ui->outputSlot->removeItem();

    if (!root.contains(QStringLiteral("experience"))) return;

    ui->experienceInput->setValue(root[QStringLiteral("experience")].toDouble());

    if (root.contains(QStringLiteral("cookingtime"))) {
        ui->cookTimeCheck->setCheckState(Qt::Checked);
        ui->cookTimeInput->setValue(root[QStringLiteral(
                                             "cookingtime")].toDouble());
    } else {
        ui->cookTimeCheck->setCheckState(Qt::Unchecked);
    }
}

void VisualRecipeEditorDock::readStonecuttingJson(const QJsonObject &root) {
    /*qDebug() << "readStonecuttingJson"; */
    if (!root.contains(QStringLiteral("ingredient"))) return;

    QJsonObject ingredient = root[QStringLiteral("ingredient")].toObject();
    QString     itemID;
    if (ingredient.contains(QStringLiteral("item"))) {
        itemID = ingredient[QStringLiteral("item")].toString();
    } else if (ingredient.contains(QStringLiteral("tag"))) {
        itemID = '#' + ingredient[QStringLiteral("tag")].toString();
    } else {
        return;
    }

    if (!itemID.isEmpty())
        ui->stonecuttingSlot->setItem(MCRInvItem(itemID));
    else
        ui->stonecuttingSlot->removeItem();
    if (!root.contains(QStringLiteral("result"))) return;

    itemID = root[QStringLiteral("result")].toString();
    if (!itemID.isEmpty())
        ui->outputSlot->setItem(MCRInvItem(itemID));
    else
        ui->outputSlot->removeItem();

    if (!root.contains(QStringLiteral("count"))) return;

    ui->resultCountInput->setValue(root[QStringLiteral("count")].toInt());
}
