#include "visualrecipeeditordock.h"
#include "ui_visualrecipeeditordock.h"

#include "inventoryitem.h"
#include "mainwindow.h"
#include "stylesheetreapplier.h"
#include "modelfunctions.h"

#include "game.h"
#include "globalhelpers.h"
#include "platforms/windows_specific.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>


VisualRecipeEditorDock::VisualRecipeEditorDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VisualRecipeEditorDock) {
    ui->setupUi(this);

    ui->outputSlot->setAcceptTag(false);
    ui->outputSlot->setAcceptMultiple(false);

    this->craftingSlots =
        QVector<InventorySlot *>({ ui->craftingSlot_1, ui->craftingSlot_2,
                                   ui->craftingSlot_3, ui->craftingSlot_4,
                                   ui->craftingSlot_5, ui->craftingSlot_6,
                                   ui->craftingSlot_7, ui->craftingSlot_8,
                                   ui->craftingSlot_9 });

    const bool from_v1_21_3 = Game::version() >= Game::v1_21_3;
    for (auto *slot: std::as_const(craftingSlots)) {
        slot->setAcceptComponents(false);
        if (from_v1_21_3) {
            slot->setAcceptItemsOrTag();
        }
    }

    connect(ui->customTabBar, &QTabBar::currentChanged,
            this, &VisualRecipeEditorDock::onRecipeTabChanged);
    connect(ui->smithTypeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            ui->outputSlot, [this](int index){
        if (index == 0) {
            ui->outputSlot->setDisabled(false);
        } else {
            ui->outputSlot->clearItems();
            ui->outputSlot->setDisabled(true);
        }
    });
    connect(ui->craftTypeInput, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int index){
        if (index == 3) {         // Transmute crafting
            for (auto *slot: std::as_const(craftingSlots)) {
                slot->setEnabled(false);
                slot->clearItems();
            }
            ui->craftingSlot_5->setEnabled(true);
            ui->craftingSlot_6->setEnabled(true);
            ui->resultCountInput->setEnabled(false);
        } else {
            for (auto *slot: std::as_const(craftingSlots)) {
                slot->setEnabled(true);
            }
            ui->resultCountInput->setEnabled(true);
        }
    });
    connect(ui->smithTypeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            ui->resultCountInput, &QWidget::setDisabled);
    connect(ui->writeRecipeBtn, &QPushButton::clicked,
            this, &VisualRecipeEditorDock::writeRecipe);
    connect(ui->readRecipeBtn, &QPushButton::clicked, this,
            &VisualRecipeEditorDock::readRecipe);
    connect(this, &QDockWidget::topLevelChanged, this, [this](bool) {
        Windows::setDarkFrameIfDarkMode(this);
        adjustSize();
    });
    ui->customTabFrame->installEventFilter(styleSheetReapplier);

    setupCustomTab();
    setupCategoryCombo();
    if (Game::version() < Game::v1_19_4) {
        ui->showNotifCheck->hide();
    }
    if (Game::version() < Game::v1_20) {
        ui->smithingSlot_2->hide();
        ui->smithTypeCombo->hide();
        ui->label_3->hide();
    } else {
        ui->smithingPlus->hide();
    }
    if (Game::version() < Game::v1_21_3) {
        hideComboRow(ui->craftTypeInput, 3);
    }
}

VisualRecipeEditorDock::~VisualRecipeEditorDock() {
    delete ui;
}

void VisualRecipeEditorDock::setupCustomTab() {
    /*Add tabs */
    ui->customTabBar->addTab(tr("Crafting"));
    ui->customTabBar->addTab(tr("Smelting"));
    ui->customTabBar->addTab(tr("Stonecutting"));
    if (Game::version() >= Game::v1_16)
        ui->customTabBar->addTab(tr("Smithing"));

    /*Make tab bar overlay top pixel of tab frame */
    ui->customTabWidgetLayout->removeWidget(ui->customTabBar);
    ui->customTabWidgetLayout->removeWidget(ui->customTabFrame);
    ui->customTabWidgetLayout->addWidget(ui->customTabBar, 0, 0, Qt::AlignTop);
    ui->customTabWidgetLayout->addWidget(ui->customTabFrame, 0, 0);
    ui->customTabBar->raise();
    ui->customTabBar->setExpanding(false);
    /*ui->customTabBar->setMovable(true); */
}

void VisualRecipeEditorDock::setupCategoryCombo() {
    if (Game::version() >= Game::v1_19_3) {
        appendRowToModel(m_craftCategories, tr("Miscellaneous"), "misc");
        appendRowToModel(m_craftCategories, tr("Building"), "building");
        appendRowToModel(m_craftCategories, tr("Equipment"), "equipment");
        appendRowToModel(m_craftCategories, tr("Redstone"), "redstone");
        appendRowToModel(m_smeltCategories, tr("Miscellaneous"), "misc");
        appendRowToModel(m_smeltCategories, tr("Blocks"), "blocks");
        appendRowToModel(m_smeltCategories, tr("Food"), "food");
        ui->recipeCategoryCombo->setModel(&m_craftCategories);
    } else {
        ui->recipeCategoryLabel->hide();
        ui->recipeCategoryCombo->hide();
    }
}

void VisualRecipeEditorDock::retranslate() {
    ui->retranslateUi(this);

    ui->customTabBar->setTabText(0, tr("Crafting"));
    ui->customTabBar->setTabText(1, tr("Smelting"));
    ui->customTabBar->setTabText(2, tr("Stonecutting"));
    if (Game::version() >= Game::v1_16)
        ui->customTabBar->setTabText(3, tr("Smithing"));

    if (Game::version() >= Game::v1_19_3) {
        Q_ASSERT(m_craftCategories.rowCount() > 0);
        m_craftCategories.item(0)->setText(tr("Miscellaneous"));
        m_craftCategories.item(1)->setText(tr("Building"));
        m_craftCategories.item(2)->setText(tr("Equipment"));
        m_craftCategories.item(3)->setText(tr("Redstone"));
        Q_ASSERT(m_smeltCategories.rowCount() > 0);
        m_smeltCategories.item(0)->setText(tr("Miscellaneous"));
        m_smeltCategories.item(1)->setText(tr("Blocks"));
        m_smeltCategories.item(2)->setText(tr("Food"));
    }
}

void VisualRecipeEditorDock::changeEvent(QEvent *event) {
    QDockWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        retranslate();
}

void VisualRecipeEditorDock::onRecipeTabChanged(int index) {
    Q_ASSERT(index >= 0 && index < 4);
    ui->stackedRecipeWidget->setCurrentIndex(index);
    ui->resultCountInput->setEnabled(index != 1);
    ui->stackedOptions->setCurrentIndex(qMin(index, 3));

    if (Game::version() >= Game::v1_19) {
        ui->recipeCategoryLabel->setEnabled(index <= 1);
        ui->recipeCategoryCombo->setEnabled(index <= 1);
        if (index == 0) {
            ui->recipeCategoryCombo->setModel(&m_craftCategories);
        } else if (index == 1) {
            ui->recipeCategoryCombo->setModel(&m_smeltCategories);
        }
    }
    if (Game::version() >= Game::v1_20) {
        ui->outputSlot->setEnabled((index != 3) ||
                                   (ui->smithTypeCombo->currentIndex() == 0));
    }
    if (Game::version() >= Game::v1_21_3) {
        ui->resultCountInput->setEnabled(ui->craftTypeInput->currentIndex() <
                                         3);
    }
}

void VisualRecipeEditorDock::writeRecipe() {
    const int   index = ui->customTabBar->currentIndex();
    QJsonObject root;

    if (!ui->recipeGroupInput->text().isEmpty())
        root.insert("group", ui->recipeGroupInput->text());

    if ((index <= 1) && (Game::version() >= Game::v1_19_3)
        && (ui->recipeCategoryCombo->currentIndex() != -1)) {
        root.insert("category", ui->recipeCategoryCombo->currentData(
                        Qt::UserRole + 1).toString());
    }
    if (Game::version() >= Game::v1_19_4) {
        ui->showNotifCheck->insertToJsonObject(root, "show_notification");
    }

    QJsonDocument jsonDoc;
    switch (index) {
        case 0:
            jsonDoc = QJsonDocument(genCraftingJson(root));
            break;

        case 1:
            jsonDoc = QJsonDocument(genSmeltingJson(root));
            break;

        case 2:
            jsonDoc = QJsonDocument(genStonecuttingJson(root));
            break;

        case 3:
            if (Game::version() >= Game::v1_16)
                jsonDoc = QJsonDocument(genSmithingJson(root));
            break;
    }

    qobject_cast<MainWindow *>(parent())->setCodeEditorText(jsonDoc.toJson());
}

QJsonObject VisualRecipeEditorDock::genCraftingJson(QJsonObject root) {
    int craftTypeIdx = ui->craftTypeInput->currentIndex();

    if (craftTypeIdx < 2) { /* Shaped crafting */
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:crafting_shaped"));

        QMap< QVector<InventoryItem>, QString> keys;
        QString                                patternStr;
        const QString                          patternChars = QStringLiteral(
            "123456789");
        int pattCharIdx = -1;
        for (auto *slot : qAsConst(craftingSlots)) {
            const auto &items = slot->getItems();
            if (!items.isEmpty()) {
                if (!keys.contains(items)) {
                    ++pattCharIdx;
                    keys.insert(items, QString(patternChars[pattCharIdx]));
                }
                patternStr += keys.value(items);
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
                for (const auto &row : pattern) {
                    width = qMax(row.toString().length(), width);
                }
                for (int y = 0; y < width; ++y) {
                    for (int x = 0; x < pattern.count(); ++x) {
                        const QChar c = pattern[y].toString()[x];
                        if (c != ' ') {
                            xstart = qMin(x, xstart);
                            xend   = qMax(x, xend);
                            ystart = qMin(y, ystart);
                            yend   = qMax(y, yend);
                        }
                    }
                }
                const int  xlen = xend - xstart + 1;
                QJsonArray tmpPattern;
                for (int y = 0; y < 3; ++y) {
                    if (y >= ystart && y <= yend) {
                        const auto &&row = pattern[y].toString();
                        tmpPattern.push_back(row.mid(xstart, xlen));
                    }
                }
                pattern = tmpPattern;
            }
        }
        root.insert("pattern", pattern);

        QJsonObject key;
        for (const auto &str : keys.toStdMap()) {
            auto items = str.first;
            key.insert(str.second, ingredientsToJson(items));
        }
        root.insert(QStringLiteral("key"), key);
    } else if (craftTypeIdx == 2) { /* Shapeless crafting */
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:crafting_shapeless"));

        QJsonArray ingredients;
        for (int i = 0; i < 9; ++i) {
            const auto &items      = this->craftingSlots[i]->getItems();
            auto        ingredient = ingredientsToJson(items);
            /*qDebug() << ingredient << ingredient.isNull(); */
            if (!ingredient.isNull())
                ingredients.push_back(ingredient);
        }
        root.insert(QStringLiteral("ingredients"), ingredients);
    } else { // Transmute crafting
        if (ui->craftingSlot_5->isEmpty() || ui->craftingSlot_6->isEmpty())
            return root;

        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:crafting_transmute"));

        const auto &input = ui->craftingSlot_5->getItems();
        root.insert(QStringLiteral("input"), ingredientsToJson(input, true));
        const auto &material = ui->craftingSlot_6->getItems();
        root.insert(QStringLiteral("material"),
                    ingredientsToJson(material, true));
        root.insert(QStringLiteral("result"),
                    ui->outputSlot->itemNamespacedID());
    }

    if (craftTypeIdx < 3) {
        QJsonObject     result;
        const QString &&itemIdField =
            (Game::version() >= Game::v1_20_6) ? "id"_QL1 : "item"_QL1;
        result.insert(itemIdField, ui->outputSlot->itemNamespacedID());
        result.insert(QStringLiteral("count"), ui->resultCountInput->value());
        if (Game::version() >= Game::v1_20_6) {
            tryWriteComponents(result, ui->outputSlot);
        }
        root.insert(QStringLiteral("result"), result);
    }

    return root;
}

QJsonObject VisualRecipeEditorDock::genSmeltingJson(QJsonObject root) {
    int index = ui->smeltBlockInput->currentIndex();

    static const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"),  QStringLiteral("campfire_cooking")
    };

    const QString &&type = QStringLiteral("minecraft:") + smeltingTypes[index];

    root.insert(QLatin1String("type"), type);

    const auto &items = ui->smeltingSlot_0->getItems();
    root.insert("ingredient", ingredientsToJson(items));

    if (Game::version() >= Game::v1_20_6) {
        QJsonObject result{
            { QStringLiteral("id"), ui->outputSlot->itemNamespacedID() }, };
        tryWriteComponents(result, ui->outputSlot);
        root.insert(QStringLiteral("result"), result);
    } else {
        root.insert(QStringLiteral("result"),
                    ui->outputSlot->itemNamespacedID());
    }
    root.insert(QStringLiteral("experience"), ui->experienceInput->value());
    if (ui->cookTimeCheck->isChecked())
        root.insert(QStringLiteral("cookingtime"), ui->cookTimeInput->value());
    return root;
}

QJsonObject VisualRecipeEditorDock::genStonecuttingJson(QJsonObject root) {
    root.insert(QStringLiteral("type"),
                QStringLiteral("minecraft:stonecutting"));

    const auto &items = ui->stonecuttingSlot->getItems();
    root.insert(QStringLiteral("ingredient"), ingredientsToJson(items));

    if (Game::version() >= Game::v1_20_6) {
        QJsonObject result{
            { QStringLiteral("id"), ui->outputSlot->itemNamespacedID() }, };
        tryWriteComponents(result, ui->outputSlot);
        root.insert(QStringLiteral("result"), result);
    } else {
        root.insert(QStringLiteral("result"),
                    ui->outputSlot->itemNamespacedID());
    }
    root.insert(QStringLiteral("count"), ui->resultCountInput->value());

    return root;
}

QJsonObject VisualRecipeEditorDock::genSmithingJson(QJsonObject root) {
    const bool from_v1_20 = Game::version() >= Game::v1_20;

    if (from_v1_20) {
        root.insert(QStringLiteral("type"),
                    (ui->smithTypeCombo->currentIndex() == 0)
                        ? QStringLiteral("minecraft:smithing_transform")
                        : QStringLiteral("minecraft:smithing_trim"));
    } else {
        root.insert(QStringLiteral("type"),
                    QStringLiteral("minecraft:smithing"));
    }

    const bool until_v1_21_3 = Game::version() < Game::v1_21_3;

    if (!ui->smithingSlot_0->isEmpty()) {
        const auto &base = ui->smithingSlot_0->getItems();
        root.insert(QStringLiteral("base"),
                    ingredientsToJson(base, from_v1_20));
    } else if (until_v1_21_3) {
        return root;
    }

    if (until_v1_21_3 || !ui->smithingSlot_1->isEmpty()) {
        const auto &addition = ui->smithingSlot_1->getItems();
        root.insert(QStringLiteral("addition"),
                    ingredientsToJson(addition, from_v1_20));
    }
    if (from_v1_20 && (until_v1_21_3 || !ui->smithingSlot_2->isEmpty())) {
        const auto &templates = ui->smithingSlot_2->getItems();
        root.insert(QStringLiteral("template"),
                    ingredientsToJson(templates, true));
    }

    if (!from_v1_20 || (ui->smithTypeCombo->currentIndex() == 0)) {
        QJsonObject     result;
        const QString &&itemIdField =
            (Game::version() >= Game::v1_20_6) ? "id"_QL1 : "item"_QL1;
        result.insert(itemIdField, ui->outputSlot->itemNamespacedID());
        result.insert(QStringLiteral("count"), ui->resultCountInput->value());
        if (Game::version() >= Game::v1_20_6) {
            tryWriteComponents(result, ui->outputSlot);
        }
        root.insert(QStringLiteral("result"), result);
    }

    return root;
}

void VisualRecipeEditorDock::readRecipe() {
    QString input = qobject_cast<MainWindow *>(parent())->getCodeEditorText();

    QJsonDocument json_doc = QJsonDocument::fromJson(input.toUtf8());

    if (json_doc.isNull() || (!json_doc.isObject()))
        return;

    QJsonObject root = json_doc.object();
    if (root.isEmpty())
        return;

    ui->resultCountInput->setValue(1);
    ui->recipeGroupInput->setText(root[QStringLiteral("group")].toString());

    static const QString craftingTypes[] = {
        QStringLiteral("crafting_shaped"), QStringLiteral(
            "crafting_shapeless"),
        QStringLiteral("crafting_transmute"),
    };

    static const QString smeltingTypes[] = {
        QStringLiteral("smelting"), QStringLiteral("blasting"),
        QStringLiteral("smoking"),  QStringLiteral("campfire_cooking"),
    };

    if (!root.contains(QStringLiteral("type"))) return;

    QString type = root[QStringLiteral("type")].toString();
    Glhp::removePrefix(type, "minecraft:"_QL1);

    for (const QString &craftingType : craftingTypes) {
        if (type == craftingType) {
            ui->customTabBar->setCurrentIndex(0);
            readCraftingJson(root);
            goto FINALIZE;
        }
    }
    for (const QString &smeltingType : smeltingTypes) {
        if (type == smeltingType) {
            ui->customTabBar->setCurrentIndex(1);
            readSmeltingJson(root);
            goto FINALIZE;
        }
    }
    if (type == QStringLiteral("stonecutting")) {
        ui->customTabBar->setCurrentIndex(2);
        readStonecuttingJson(root);
    } else if (type == QStringLiteral("smithing")
               && (Game::version() >= Game::v1_16)
               && (Game::version() < Game::v1_20)) {
        ui->customTabBar->setCurrentIndex(3);
        readSmithingJson(root);
    } else if ((type == QStringLiteral("smithing_transform") ||
                type == QStringLiteral("smithing_trim"))
               && (Game::version() >= Game::v1_20)) {
        ui->customTabBar->setCurrentIndex(3);
        readSmithingJson(root);
    }

 FINALIZE:
    if (root.contains(QStringLiteral("category"))
        && (Game::version() >= Game::v1_19_3)) {
        const QString &&category = root[QStringLiteral("category")].toString();
        const int       index    = ui->recipeCategoryCombo->findData(
            category, Qt::UserRole + 1);
        ui->recipeCategoryCombo->setCurrentIndex(index);
    }
    if (Game::version() >= Game::v1_19_4) {
        ui->showNotifCheck->setupFromJsonObject(root, "show_notification");
    }
}

void VisualRecipeEditorDock::readCraftingJson(const QJsonObject &root) {
    QString type = root[QStringLiteral("type")].toString();

    Glhp::removePrefix(type, "minecraft:"_QL1);

    if (type == QStringLiteral("crafting_shaped")) {
        if (!root.contains(QStringLiteral("pattern"))) return;

        QJsonArray pattern = root[QStringLiteral("pattern")].toArray();

        if (pattern.count() > 3) return;

        int rowlen = pattern[0].toString().length();
        for (const auto &row : qAsConst(pattern))
            if (row.toString().length() != rowlen) return;

        if (pattern.count() == 3 && rowlen == 3)
            ui->craftTypeInput->setCurrentIndex(0);
        else
            ui->craftTypeInput->setCurrentIndex(1);

        QString patternStr;
        for (const auto &row : qAsConst(pattern))
            patternStr += row.toString().leftJustified(3, ' ');
        patternStr = patternStr.leftJustified(9, ' ');

        if (!root.contains(QStringLiteral("key"))) return;

        QJsonObject keys = root[QStringLiteral("key")].toObject();

        for (int i = 0; i < 9; ++i) {
            QString key = QString(patternStr[i]);
            if (key == ' ') {
                craftingSlots[i]->clearItems();
            } else {
                if (!keys.contains(key)) continue;
                QJsonValue keyVal = keys[key];

                craftingSlots[i]->setItems(JsonToIngredients(keyVal));
            }
        }
    } else if (type == QStringLiteral("crafting_shapeless")) {
        if (!root.contains(QStringLiteral("ingredients"))) return;

        QJsonArray ingredients = root[QStringLiteral("ingredients")].toArray();
        if (ingredients.count() > 9 || ingredients.count() < 1) return;

        ui->craftTypeInput->setCurrentIndex(2);

        for (int i = 0; i < 9; ++i) {
            if (i >= ingredients.count()) {
                craftingSlots[i]->clearItems();
            } else {
                QJsonValue ingredient = ingredients[i];
                craftingSlots[i]->setItems(JsonToIngredients(ingredient));
            }
        }
    } else if (type == QStringLiteral("crafting_transmute")) {
        if (Game::version() < Game::v1_21_3) return;

        ui->craftTypeInput->setCurrentIndex(3);

        if (!root.contains(QStringLiteral("input"))) return;

        QJsonValue input = root[QStringLiteral("input")];
        ui->craftingSlot_5->setItems(JsonToIngredients(input));

        if (!root.contains(QStringLiteral("material"))) return;

        QJsonValue material = root[QStringLiteral("material")];
        ui->craftingSlot_6->setItems(JsonToIngredients(material));

        if (!root.contains(QStringLiteral("result"))) return;

        ui->outputSlot->setItem(root[QStringLiteral("result")].toString());
    }

    if (type != QStringLiteral("crafting_transmute")) {
        if (Game::version() < Game::v1_21_3) return;

        if (!root.contains(QStringLiteral("result"))) return;

        QJsonObject     result      = root[QStringLiteral("result")].toObject();
        const QString &&itemIdField =
            (Game::version() >= Game::v1_20_6) ? "id"_QL1 : "item"_QL1;
        if (!result.contains(itemIdField)) return;

        QString itemID = result[itemIdField].toString();
        if (!itemID.isEmpty())
            ui->outputSlot->setItem(InventoryItem(itemID));
        else
            ui->outputSlot->clearItems();
        ui->resultCountInput->setValue(result[QStringLiteral("count")].toInt(1));
        tryReadComponents(result, ui->outputSlot);
    }
}

void VisualRecipeEditorDock::readSmeltingJson(const QJsonObject &root) {
    QString type =
        root[QStringLiteral("type")].toString();
    static const QString smeltingTypes[] = {
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

    QJsonValue ingredient = root[QStringLiteral("ingredient")];

    ui->smeltingSlot_0->setItems(JsonToIngredients(ingredient));
    if (!root.contains(QStringLiteral("result"))) return;

    QString itemID;
    if (Game::version() >= Game::v1_20_6) {
        QJsonObject result = root[QStringLiteral("result")].toObject();
        if (!result.contains(QStringLiteral("id"))) return;

        itemID = result[QStringLiteral("id")].toString();
        tryReadComponents(result, ui->outputSlot);
    } else {
        itemID = root[QStringLiteral("result")].toString();
    }
    if (!itemID.isEmpty())
        ui->outputSlot->setItem(InventoryItem(itemID));
    else
        ui->outputSlot->clearItems();

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

    QJsonValue ingredient = root[QStringLiteral("ingredient")];
    ui->stonecuttingSlot->setItems(JsonToIngredients(ingredient));

    if (!root.contains(QStringLiteral("result"))) return;

    QString itemID;
    if (Game::version() >= Game::v1_20_6) {
        QJsonObject result = root[QStringLiteral("result")].toObject();
        if (!result.contains(QStringLiteral("id"))) return;

        itemID = result[QStringLiteral("id")].toString();
        tryReadComponents(result, ui->outputSlot);
    } else {
        itemID = root[QStringLiteral("result")].toString();
    }
    if (!itemID.isEmpty())
        ui->outputSlot->setItem(InventoryItem(itemID));
    else
        ui->outputSlot->clearItems();

    if (!root.contains(QStringLiteral("count"))) return;

    ui->resultCountInput->setValue(root[QStringLiteral("count")].toInt());
}

void VisualRecipeEditorDock::readSmithingJson(const QJsonObject &root) {
    bool isArmorTrimMode = false;

    if (Game::version() < Game::v1_20) {
        if (!root.contains(QStringLiteral("base"))) return;
    } else {
        QString type = root.value("type").toString();
        Glhp::removePrefix(type, "minecraft:"_QL1);
        isArmorTrimMode = type == "smithing_trim"_QL1;
        ui->smithTypeCombo->setCurrentIndex(isArmorTrimMode);
    }
    const QJsonValue &&base = root[QStringLiteral("base")];
    ui->smithingSlot_0->setItems(JsonToIngredients(base));

    if (Game::version() < Game::v1_20) {
        if (!root.contains(QStringLiteral("addition"))) return;
    }
    const QJsonValue &&addition = root[QStringLiteral("addition")];
    ui->smithingSlot_1->setItems(JsonToIngredients(addition));

    if (root.contains("template")) {
        const QJsonValue &&templates = root[QStringLiteral("template")];
        ui->smithingSlot_2->setItems(JsonToIngredients(templates));
    } else {
        ui->smithingSlot_2->clearItems();
    }

    if (!isArmorTrimMode) {
        if (!root.contains(QStringLiteral("result"))) return;

        const QJsonObject &&result =
            root[QStringLiteral("result")].toObject();
        const QString &&itemIdField =
            (Game::version() >= Game::v1_20_6) ? "id"_QL1 : "item"_QL1;
        if (!result.contains(itemIdField)) return;

        const QString &&itemID = result[itemIdField].toString();
        if (!itemID.isEmpty())
            ui->outputSlot->setItem(InventoryItem(itemID));
        else
            ui->outputSlot->clearItems();

        if (result.contains(QStringLiteral("count"))) {
            ui->resultCountInput->setValue(result[QStringLiteral(
                                                      "count")].toInt());
        }
        tryReadComponents(result, ui->outputSlot);
    }
}

QVector<InventoryItem> VisualRecipeEditorDock::JsonToIngredients(
    const QJsonValue &keyVal) {
    QJsonArray keyArray;

    if (keyVal.isObject() || keyVal.isString()) {
        keyArray.push_back(keyVal);
    } else {
        keyArray = keyVal.toArray();
    }

    QVector<InventoryItem> items;
    for (const auto &key : qAsConst(keyArray)) {
        QString itemID;

        if (Game::version() >= Game::v1_21_3) {
            if (!key.isString()) {
                qWarning() <<
                    "JsonToIngredients: JSON ingredient has to be a string of namespaced ID or ID tag.";
            }
            itemID = key.toString();
            if (itemID.startsWith('#') && keyVal.isArray()) {
                QMessageBox::critical(this,
                                      tr("JSON input error"),
                                      tr(
                                          "Item tags are not allowed in the ingredient list."));
                return {};
            }
        } else {
            QJsonObject &&keyJson = key.toObject();
            if (keyJson.contains(QStringLiteral("item"))) {
                itemID = keyJson[QStringLiteral("item")].toString();
            } else if (keyJson.contains(QStringLiteral("tag"))) {
                itemID = '#' +
                         keyJson[QStringLiteral("tag")].toString();
            } else {
                qWarning() <<
                    "JsonToIngredients: JSON ingredient has no 'item' nor 'tag' key.";
            }
        }
        /*qDebug() << keyJson << keyJson.contains("item") << itemID; */
        if (!itemID.isEmpty() &&
            (!items.contains(InventoryItem(itemID)))) {
            items.push_back(InventoryItem(itemID));
        }
    }
    return items;
}

QJsonValue VisualRecipeEditorDock::ingredientsToJson(
    const QVector<InventoryItem> &items, const bool emptyAsArray) {
    QJsonArray keyItems;

    for (const auto &item : items) {
        QString &&itemID = item.getNamespacedID();
        if (Game::version() >= Game::v1_21_3) {
            keyItems.push_back(itemID);
        } else {
            QJsonObject keyItem;
            if (item.isTag())
                keyItem.insert(QStringLiteral("tag"), itemID.remove(0, 1));
            else
                keyItem.insert(QStringLiteral("item"), itemID);
            keyItems.push_back(keyItem);
        }
    }
    /*qDebug() << keyItems.isEmpty() << keyItems.count() << keyItems; */
    if (keyItems.isEmpty()) {
        return emptyAsArray ? QJsonArray() : QJsonValue();
    } else if (keyItems.count() == 1) {
        return keyItems.at(0);
    } else {
        return keyItems;
    }
}


void VisualRecipeEditorDock::tryWriteComponents(QJsonObject &obj,
                                                InventorySlot *slot) {
    const auto &components = slot->itemComponents();

    if (!components.isEmpty()) {
        obj.insert(QStringLiteral("components"),
                   QJsonObject::fromVariantMap(components));
    }
}

void VisualRecipeEditorDock::tryReadComponents(const QJsonObject &obj,
                                               InventorySlot *slot) {
    if (obj.contains(QStringLiteral("components")) &&
        !slot->isEmpty()) {
        slot->getItem().setComponents(
            obj[QStringLiteral("components")].toObject().toVariantMap());
        emit slot->itemChanged();
    }
}
