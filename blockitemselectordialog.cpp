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
    filterModel.setSourceModel(model);
    filterModel.setFilterRole(Qt::UserRole + 2);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    //ui->listView->setModel(model);
    ui->listView->setModel(&filterModel);

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
            continue;
        }
        auto key = (blockIter != MCRBlockInfo->constEnd())
                       ? blockIter.key() : itemIter.key();
        MCRInvItem invItem(this, key);
        invItem.setAutoFillBackground(true);
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(*invItem.pixmap()));
        item->setSizeHint(QSize(32 + (2*2), 32 + (2*2)));
        item->setData(invItem.namespacedID, Qt::UserRole + 1);
        item->setData(QString("%1|(%2)")
                          .arg(invItem.namespacedID,QRegularExpression
                               ::escape(invItem.getName().toLower())),
                      Qt::UserRole + 2);
        qDebug() << QString("%1|(%2)")
                        .arg(invItem.namespacedID,QRegularExpression
                             ::escape(invItem.getName().toLower()));
        item->setToolTip(invItem.getName());
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

    filterModel.setFilterRegularExpression(input);
//    //int c = 0;
//    for(int i = 0; i < model->rowCount(); ++i) {
//        QModelIndex index = model->index(i, 0);
//        auto item = model->itemFromIndex(index);
//        MCRInvItem invItem(this, item->data().toString());

//        if(invItem.getName().toLower().contains(input.toLower())
//            || invItem.namespacedID.contains(input.toLower())) {
//            //++c;
//            if(ui->listView->isRowHidden(i))
//                ui->listView->setRowHidden(i, false);
//        } else {
//            if(!ui->listView->isRowHidden(i))
//                ui->listView->setRowHidden(i, true);
//        }
//    }

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
    auto *item = model->itemFromIndex(filterModel.mapToSource(indexes[0]));
    return item->data().toString();
}

void BlockItemSelectorDialog::checkOK() {
    qDebug() << getSelectedID();
    if(getSelectedID().isEmpty())
        selectButton->setEnabled(false);
    else
        selectButton->setEnabled(true);
}
