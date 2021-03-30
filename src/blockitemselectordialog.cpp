#include "blockitemselectordialog.h"
#include "ui_blockitemselectordialog.h"

#include "mainwindow.h"

#include <QDebug>
#include <QIcon>

BlockItemSelectorDialog::BlockItemSelectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BlockItemSelectorDialog) {
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
    setupListView();
    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            [ = ](const QString &input) {
        filterModel.setFilterRegularExpression(input);
    });
    connect(ui->filterByBlockCheck, &QCheckBox::toggled, [ = ](bool checked) {
        filterModel.setFilterByBlock(checked);
    });
    connect(ui->filterByItemCheck, &QCheckBox::toggled, [ = ](bool checked) {
        filterModel.setFilterByItem(checked);
    });
    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &BlockItemSelectorDialog::checkOK);

    selectButton = new QPushButton(tr("Select"), this);
    ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Ok));
    ui->buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    connect(selectButton,
            &QPushButton::clicked,
            this,
            &BlockItemSelectorDialog::accept);

    checkOK();
}

void BlockItemSelectorDialog::setupListView() {
    const int itemPixmapSize = 32;

    model.setParent(ui->listView);
    filterModel.setSourceModel(&model);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listView->setModel(&filterModel);

    const auto &MCRItemInfo  = MainWindow::getMCRInfo("item");
    const auto &MCRBlockInfo = MainWindow::getMCRInfo("block");

    QMap<QString,
         QVariant>::const_iterator blockIter = MCRBlockInfo.constBegin();
    QMap<QString,
         QVariant>::const_iterator itemIter = MCRItemInfo.constBegin();
    int                            c        = 0;
    while ((blockIter != MCRBlockInfo.constEnd())
           || (itemIter != MCRItemInfo.constEnd())) {
        if (blockIter.value().toMap().contains("unobtainable")) {
            if (blockIter != MCRBlockInfo.constEnd())
                ++blockIter;
            else
                ++itemIter;
            continue;
        }
        auto key = (blockIter != MCRBlockInfo.constEnd())
                       ? blockIter.key() : itemIter.key();
        MCRInvItem invItem(key);
        auto      *item = new QStandardItem();
        item->setIcon(QIcon(invItem.getPixmap()));
        item->setSizeHint(QSize(itemPixmapSize + (3 * 2),
                                itemPixmapSize + (3 * 2)));
        QVariant vari;
        vari.setValue(invItem);
        item->setData(vari, Qt::UserRole + 1);
        item->setToolTip(invItem.toolTip());
        model.appendRow(item);
        ++c;
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

    if (indexes.isEmpty()) return "";

    QStandardItem *item =
        model.itemFromIndex(filterModel.mapToSource(indexes[0]));
    auto invItem = item->data(Qt::UserRole + 1).value<MCRInvItem>();
    return invItem.getNamespacedID();
}

QVector<MCRInvItem> BlockItemSelectorDialog::getSelectedItems() {
    QVector<MCRInvItem> items;

    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return items;

    for (auto index : indexes) {
        QStandardItem *item =
            model.itemFromIndex(filterModel.mapToSource(index));
        auto invItem = item->data(Qt::UserRole + 1).value<MCRInvItem>();
        if (items.contains(invItem)) continue;
        items.push_back(invItem);
    }
    return items;
}

void BlockItemSelectorDialog::checkOK() {
    if (getSelectedID().isEmpty())
        selectButton->setEnabled(false);
    else
        selectButton->setEnabled(true);
}

bool BlockItemSelectorDialog::getAllowMultiItems() const {
    return allowMultiItems;
}

void BlockItemSelectorDialog::setAllowMultiItems(bool value) {
    allowMultiItems = value;
    if (!allowMultiItems)
        ui->listView->setSelectionMode(QListView::SingleSelection);
}
