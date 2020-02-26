#include "blockitemselectordialog.h"
#include "ui_blockitemselectordialog.h"

#include "mcrinvitem.h"
#include "mainwindow.h"

#include <QDebug>
#include <QIcon>

BlockItemSelectorDialog::BlockItemSelectorDialog(QWidget *parent) :
      QDialog(parent),
      ui(new Ui::BlockItemSelectorDialog)
{
    ui->setupUi(this);
    setupTreeView();
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
            &BlockItemSelectorDialog::onListViewSearch);
    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &BlockItemSelectorDialog::checkOK);

    selectButton = new QPushButton(tr("Select"), this);
    ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Ok));
    ui->buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    connect(selectButton, &QPushButton::clicked, this, &BlockItemSelectorDialog::accept);

    checkOK();
}

void BlockItemSelectorDialog::setupTreeView() {
    model->setParent(ui->listView);
    ui->listView->setModel(model);

    auto MCRItemInfo = MainWindow::getMCRInfo("item");
    auto MCRBlockInfo = MainWindow::getMCRInfo("block");
    QMap<QString, QVariant>::const_iterator blockIter = MCRBlockInfo->constBegin();
    QMap<QString, QVariant>::const_iterator itemIter = MCRItemInfo->constBegin();
    //int c = 0;
    while ((blockIter != MCRBlockInfo->constEnd())
           || (itemIter != MCRItemInfo->constEnd())) {
        if(blockIter.value().toMap().contains("unobtainable")) {
            if(blockIter != MCRBlockInfo->constEnd())
                ++blockIter;
            else
                ++itemIter;
        }
        auto key = (blockIter != MCRBlockInfo->constEnd())
                       ? blockIter.key() : itemIter.key();
        MCRInvItem invItem(this, key);
        invItem.setAutoFillBackground(true);
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(*invItem.pixmap()));
        item->setData(invItem.namespacedID);
        model->appendRow(item);
        //++c;
        if(blockIter != MCRBlockInfo->constEnd())
            ++blockIter;
        else
            ++itemIter;
    }
}

BlockItemSelectorDialog::~BlockItemSelectorDialog()
{
    delete ui;
}

void BlockItemSelectorDialog::onListViewSearch(const QString &input) {
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    //int c = 0;
    for(int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        auto item = model->itemFromIndex(index);
        MCRInvItem invItem(this, item->data().toString());

        if(invItem.getName().toLower().contains(input.toLower())
            || invItem.namespacedID.contains(input.toLower())) {
            //++c;
            if(ui->listView->isRowHidden(i))
                ui->listView->setRowHidden(i, false);
        } else {
            if(!ui->listView->isRowHidden(i))
                ui->listView->setRowHidden(i, true);
        }
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    //    auto bs = (c+1) / 10;
    //    qDebug() << bs;
    //    ui->listView->setBatchSize(bs);
}

QString BlockItemSelectorDialog::getSelectedID() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();
    if(indexes.isEmpty()) return "";
    auto *item = model->itemFromIndex(indexes[0]);
    return item->data().toString();
}

void BlockItemSelectorDialog::checkOK() {
    qDebug() << getSelectedID();
    if(getSelectedID().isEmpty())
        selectButton->setEnabled(false);
    else
        selectButton->setEnabled(true);
}
