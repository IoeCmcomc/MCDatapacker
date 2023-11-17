#include "inventorysloteditor.h"
#include "ui_inventorysloteditor.h"

#include "inventoryslot.h"
#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
#include "globalhelpers.h"

#include <QMenu>
#include <QScreen>
#include <QMouseEvent>

InventorySlotEditor::InventorySlotEditor(InventorySlot *parent) :
    QFrame(parent),
    ui(new Ui::InventorySlotEditor) {
/*    qDebug() << "InventorySlotEditor::InventorySlotEditor"; */
    ui->setupUi(this);
    this->slot    = parent;
    this->initPos = QCursor::pos();

    setWindowFlags(Qt::Popup);

    if (slot->getAcceptTag()) {
        auto *newBtnMenu = new QMenu(ui->newButton);
        newBtnMenu->addAction(tr("Item..."), this,
                              &InventorySlotEditor::onNewItem);
        newBtnMenu->addAction(tr("Item tag..."), this,
                              &InventorySlotEditor::onNewItemTag);
        ui->newButton->setMenu(newBtnMenu);
    } else {
        ui->newButton->setText(tr("New item..."));
        auto connection = connect(ui->newButton,
                                  &QPushButton::clicked, this,
                                  &InventorySlotEditor::onNewItem);
    }

    for (const auto &invItem : qAsConst(slot->getItems())) {
        appendItem(invItem);
    }
    ui->listView->setModel(&model);
    ui->groupBox->setTitle(tr("Items") +
                           QString(" (%1)").arg(model.rowCount()));

    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &InventorySlotEditor::checkRemove);
    connect(ui->removeButton, &QPushButton::clicked,
            this, &InventorySlotEditor::onRemoveItem);

    checkRemove();
}

InventorySlotEditor::~InventorySlotEditor() {
    delete ui;
}

void InventorySlotEditor::show() {
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

    ui->groupBox->setTitle(tr("Items (%1)").arg(model.rowCount()));

    QFrame::show();
}

void InventorySlotEditor::mousePressEvent(QMouseEvent *event) {
/*    qDebug() << "mousePressEvent" << event; */
    if ((!rect().contains(event->pos()))
        || (event->buttons() ^ Qt::LeftButton)) {
        close();
    }
    QFrame::mousePressEvent(event);
}

void InventorySlotEditor::onNewItem() {
    BlockItemSelectorDialog dialog(this);

    if (dialog.exec()) {
        auto newItems = dialog.getSelectedItems();
        for (const auto &newItem : newItems) {
            if (!slot->getItems().contains(newItem)
                && slot->checkAcceptableItem(newItem)) {
                slot->appendItem(newItem);
                appendItem(newItem);
            }
        }
    }
    show();
}

void InventorySlotEditor::onNewItemTag() {
    TagSelectorDialog dialog(this, CodeFile::ItemTag);

    if (dialog.exec()) {
        InventoryItem invItem(dialog.getSelectedID());
        if ((!slot->getItems().contains(invItem)) && slot->getAcceptTag()) {
            slot->appendItem(invItem);
            if (!slot->getItems().contains(invItem)) return;

            appendItem(invItem);
        }
    }
    show();
}

void InventorySlotEditor::checkRemove() {
    const auto indexes = ui->listView->selectionModel()->selectedIndexes();

    ui->removeButton->setDisabled(indexes.isEmpty());
}

void InventorySlotEditor::onRemoveItem() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return;

    std::sort(indexes.begin(), indexes.end(),
              [](const QModelIndex &a, const QModelIndex &b)->bool {
        return a.row() > b.row();
    }); /* sort from bottom to top */

    for (const auto &index : qAsConst(indexes)) {
        int            i       = index.row();
        QStandardItem *item    = model.itemFromIndex(index);
        const auto   &&invItem =
            item->data(Qt::UserRole + 1).value<InventoryItem>();

        if (!slot->getItems().contains(invItem)) continue;
        slot->removeItem(invItem);
        if (slot->getItems().contains(invItem)) continue;
        model.removeRow(i);
    }

    show();
}

void InventorySlotEditor::close() {
    slot->update();
    deleteLater();
}

void InventorySlotEditor::appendItem(const InventoryItem &invItem) {
    auto *item = new QStandardItem();

    item->setIcon(QIcon(invItem.getPixmap()));
    item->setSizeHint(QSize(32 + (3 * 2), 32 + (3 * 2)));
    QVariant vari;
    vari.setValue(invItem);
    item->setData(vari, Qt::UserRole + 1);
    item->setToolTip(invItem.toolTip());
    model.appendRow(item);
}
