#include "mcrinvsloteditor.h"
#include "ui_mcrinvsloteditor.h"

#include "mcrinvslot.h"
#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
#include "mainwindow.h"
#include "globalhelpers.h"

#include <QStandardItem>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QScreen>
#include <QMouseEvent>
#include <QMimeData>

MCRInvSlotEditor::MCRInvSlotEditor(MCRInvSlot *parent) :
    QFrame(parent),
    ui(new Ui::MCRInvSlotEditor) {
/*    qDebug() << "MCRInvSlotEditor::MCRInvSlotEditor"; */
    ui->setupUi(this);
    this->slot    = parent;
    this->initPos = QCursor::pos();

    setWindowFlags(Qt::Popup);

    if (slot->getAcceptTag()) {
        QMenu *newBtnMenu = new QMenu(ui->newButton);
        newBtnMenu->addAction(tr("Item..."), this,
                              &MCRInvSlotEditor::onNewItem);
        newBtnMenu->addAction(tr("Item tag..."),
                              this,
                              &MCRInvSlotEditor::onNewItemTag);
        ui->newButton->setMenu(newBtnMenu);
    } else {
        ui->newButton->setText(tr("New item..."));
        auto connection = connect(ui->newButton,
                                  &QPushButton::clicked,
                                  this,
                                  &MCRInvSlotEditor::onNewItem);
    }

    for (auto invItem : slot->getItems()) {
        appendItem(invItem);
    }
    ui->listView->setModel(&model);
    ui->groupBox->setTitle(tr("Items") +
                           QString(" (%1)").arg(model.rowCount()));

    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MCRInvSlotEditor::checkRemove);
    connect(ui->removeButton,
            &QPushButton::clicked,
            this,
            &MCRInvSlotEditor::onRemoveItem);

    checkRemove();
}

MCRInvSlotEditor::~MCRInvSlotEditor() {
/*    qDebug() << this << "is going to be deleted."; */
    delete ui;
}

void MCRInvSlotEditor::show() {
    adjustSize();
    resize(width() * qMin((model.rowCount() / 8) + 1, 3),
           height() * qMin((model.rowCount() / 32) + 1, 3));

    auto   scrSize  = qApp->screens()[0]->size();
    QPoint newPoint = initPos;

    if ((newPoint.x() + width()) > scrSize.width())
        newPoint.rx() = scrSize.width() - width();
    if ((newPoint.y() + height()) > scrSize.height())
        newPoint.ry() = scrSize.height() - height();

    newPoint.rx() = qMax(newPoint.x(), 0);
    newPoint.ry() = qMax(newPoint.y(), 0);

    move(newPoint);

    QFrame::show();
}

void MCRInvSlotEditor::mousePressEvent(QMouseEvent *event) {
/*    qDebug() << "mousePressEvent" << event; */
    if ((!rect().contains(event->pos()))
        || (event->buttons() ^ Qt::LeftButton)) {
        slot->update();
        deleteLater();
    }
    QFrame::mousePressEvent(event);
}

void MCRInvSlotEditor::onNewItem() {
    BlockItemSelectorDialog dialog(this);

    if (dialog.exec()) {
        auto newItems = dialog.getSelectedItems();
        for (auto newItem : newItems) {
            /*qDebug() << newItem << slot->getItems().contains(newItem); */
            if (!slot->getItems().contains(newItem)) {
                slot->appendItem(newItem);
                appendItem(newItem);
            }
        }
    }
    ui->groupBox->setTitle(tr("Items") +
                           QString(" (%1)").arg(model.rowCount()));
    show();
}

void MCRInvSlotEditor::onNewItemTag() {
    TagSelectorDialog dialog(this, MainWindow::ItemTag);

    if (dialog.exec()) {
        auto invItem = MCRInvItem(dialog.getSelectedID());
        if ((!slot->getItems().contains(invItem)) && slot->getAcceptTag()) {
            slot->appendItem(invItem);
            if (!slot->getItems().contains(invItem)) return;

            appendItem(invItem);
        }
    }
    ui->groupBox->setTitle(tr("Items") +
                           QString(" (%1)").arg(model.rowCount()));
    show();
}

void MCRInvSlotEditor::checkRemove() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    ui->removeButton->setDisabled(indexes.isEmpty());
}

void MCRInvSlotEditor::onRemoveItem() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return;

    std::sort(indexes.begin(), indexes.end(),
              [](const QModelIndex& a, const QModelIndex& b)->bool {
        return a.row() > b.row();
    }); /* sort from bottom to top */

    for (auto index : indexes) {
        auto           i       = index.row();
        QStandardItem *item    = model.itemFromIndex(index);
        MCRInvItem     invItem =
            item->data(Qt::UserRole + 1).value<MCRInvItem>();

        if (!slot->getItems().contains(invItem)) continue;
        slot->removeItem(invItem);
        if (slot->getItems().contains(invItem)) continue;
        model.removeRow(i);
    }

    show();
    ui->groupBox->setTitle(tr("Items") +
                           QString(" (%1)").arg(model.rowCount()));
}

void MCRInvSlotEditor::appendItem(const MCRInvItem &invItem) {
    QStandardItem *item = new QStandardItem();

    item->setIcon(QIcon(invItem.getPixmap()));
    item->setSizeHint(QSize(32 + (3 * 2), 32 + (3 * 2)));
    QVariant vari;
    vari.setValue(invItem);
    item->setData(vari, Qt::UserRole + 1);
    item->setToolTip(invItem.toolTip());
    model.appendRow(item);

    /*qDebug() << item << invItem << model.rowCount(); */
}
