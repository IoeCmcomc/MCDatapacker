#include "blockitemselectordialog.h"
#include "ui_blockitemselectordialog.h"

#include "game.h"

#include <QDebug>
#include <QIcon>

BlockItemSelectorDialog::BlockItemSelectorDialog(QWidget *parent,
                                                 SelectCategory category)
    : QDialog(parent), ui(new Ui::BlockItemSelectorDialog) {
    ui->setupUi(this);
    /*
       setStyleSheet(
       "#listView {"
       "   background-color: #8B8B8B;"
       "}"
       "#listView::item {"
       "   border: 2px solid;"
       "   border-color: #373737 #FFF #FFF #373737;"
       "}"
       "#listView::item:hover {"
       "   background-color: rgba(255, 255, 255, .4);"
       "}"
       "#listView::item:selected {"
       "   border: 2px solid white;"
       "}"
       );
     */

    filterModel.setParent(ui->listView);
    model.setParent(this);
    setCategory(category);
    setupListView();
    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            this, [ = ](const QString &input) {
        filterModel.setFilterRegularExpression(input);
    });
    connect(ui->filterByBlockCheck, &QCheckBox::toggled, this,
            [ = ](bool checked) {
        filterModel.filtersRef().setFlag(FilterModel::BlockItems, checked);
    });
    connect(ui->filterByItemCheck,
            &QCheckBox::toggled,
            this,
            [ = ](bool checked) {
        filterModel.filtersRef().setFlag(FilterModel::NonblockItem, checked);
    });
    connect(ui->obtainableBlocksCheck, &QCheckBox::toggled,
            this, [ = ](bool checked) {
        filterModel.filtersRef().setFlag(FilterModel::ObtainableBlocks,
                                         checked);
    });
    connect(ui->unobtainableBlocksCheck, &QCheckBox::toggled,
            this, [ = ](bool checked) {
        filterModel.filtersRef().setFlag(FilterModel::UnobtainableBlocks,
                                         checked);
    });


    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &BlockItemSelectorDialog::checkOK);

    selectButton = new QPushButton(tr("Select"), this);
    ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Ok));
    ui->buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    connect(selectButton, &QPushButton::clicked, this,
            &BlockItemSelectorDialog::accept);

    checkOK();
}

void BlockItemSelectorDialog::setupListView() {
    constexpr int itemPixmapSize = 32 + (3 * 2);

    model.setParent(ui->listView);
    filterModel.setSourceModel(&model);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listView->setModel(&filterModel);

    const auto &&MCRItemInfo  = Game::getInfo(QStringLiteral("item"));
    const auto &&MCRBlockInfo = Game::getInfo(QStringLiteral("block"));

    auto blockIter = MCRBlockInfo.constBegin();
    auto itemIter  = (m_category == SelectCategory::Blocks)
                        ? MCRItemInfo.constEnd() : MCRItemInfo.constBegin();
    while ((blockIter != MCRBlockInfo.constEnd())
           || (itemIter != MCRItemInfo.constEnd())) {
        const auto &blockIterVal = blockIter.value();
        if ((blockIterVal.type() == QVariant::Map)
            && (blockIterVal.toMap().contains(QStringLiteral("unobtainable"))))
        {
            if (m_category == SelectCategory::ObtainableItems) {
                /* This block doesn't have a item form and
                 * the selectCategory is ObtainableItems,
                 * so this block will be skipped. */
                if ((blockIter != MCRBlockInfo.constEnd())) {
                    ++blockIter;
                } else {
                    ++itemIter;
                }
                continue;
            }
        }
        const QString &key = (blockIter != MCRBlockInfo.constEnd())
                       ? blockIter.key() : itemIter.key();
        InventoryItem invItem(key);
        auto         *item = new QStandardItem();
        item->setIcon(QIcon(invItem.getPixmap()));
        item->setSizeHint(QSize(itemPixmapSize, itemPixmapSize));
        const QVariant &&vari = QVariant::fromValue(invItem);
        item->setData(vari, Qt::UserRole + 1);
        item->setToolTip(invItem.toolTip());
        /*qDebug() << item << vari; */
        model.appendRow(item);
        if (blockIter != MCRBlockInfo.constEnd())
            ++blockIter;
        else
            ++itemIter;
    }
}

BlockItemSelectorDialog::~BlockItemSelectorDialog() {
    delete ui;
}

QString BlockItemSelectorDialog::getSelectedID() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    ui->listView->setParent(this);

    if (indexes.isEmpty()) return QString();

    QStandardItem *item =
        model.itemFromIndex(filterModel.mapToSource(indexes[0]));
    auto &&invItem = item->data(Qt::UserRole + 1).value<InventoryItem>();
    return invItem.getNamespacedID();
}

QVector<InventoryItem> BlockItemSelectorDialog::getSelectedItems() {
    QVector<InventoryItem> items;

    const auto &&indexes = ui->listView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return items;

    for (const auto &index : qAsConst(indexes)) {
        QStandardItem *item =
            model.itemFromIndex(filterModel.mapToSource(index));
        auto &&invItem =
            item->data(Qt::UserRole + 1).value<InventoryItem>();
        if (items.contains(invItem)) continue;
        items.push_back(invItem);
    }
    return items;
}

void BlockItemSelectorDialog::checkOK() {
    selectButton->setEnabled(!getSelectedID().isEmpty());
}

void BlockItemSelectorDialog::setCategory(const SelectCategory &category) {
    m_category   = category;
    using Filter = InventoryItemFilterModel::Filter;
    switch (m_category) {
        case SelectCategory::Blocks: {
            filterModel.setFilters(Filter::ObtainableBlocks);
            break;
        }

        case SelectCategory::ObtainableItems: {
            filterModel.setFilters(Filter::BlockItems | Filter::NonblockItem);
            break;
        }
    }
    ui->stackedWidget->setCurrentIndex((int)m_category);
}

void BlockItemSelectorDialog::setAllowMultiple(bool value) {
    allowMultiple = value;
    if (!allowMultiple)
        ui->listView->setSelectionMode(QListView::SingleSelection);
}
