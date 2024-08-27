#include "inventorysloteditor.h"
#include "ui_inventorysloteditor.h"

#include "inventoryslot.h"
#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
// #include "globalhelpers.h"

#include <QMenu>
#include <QScreen>
#include <QMouseEvent>
#include <QMessageBox>

InventorySlotEditor::InventorySlotEditor(InventorySlot *parent)
    : QFrame(parent), ui(new Ui::InventorySlotEditor) {
/*    qDebug() << "InventorySlotEditor::InventorySlotEditor"; */
    ui->setupUi(this);
    this->slot    = parent;
    this->initPos = QCursor::pos();
    m_itemsOnly   =
        parent->selectCategory() ==
        InventorySlot::SelectCategory::ObtainableItems;

    setWindowFlags(Qt::Popup);

    for (const auto &invItem : qAsConst(slot->getItems())) {
        appendItem(invItem);
    }

    if (slot->getAcceptTag()) {
        auto *newBtnMenu = new QMenu(ui->newButton);
        newBtnMenu->addAction(m_itemsOnly ? tr("Item...") : tr("Block..."),
                              this,
                              &InventorySlotEditor::onNewItem);
        newBtnMenu->addAction(m_itemsOnly ? tr("Item tag...")
                                        : tr("Block tag..."), this,
                              &InventorySlotEditor::onNewItemTag);
        ui->newButton->setMenu(newBtnMenu);
    } else {
        ui->newButton->setText(m_itemsOnly ? tr("New item...")
                                         : tr("New block..."));
        auto connection = connect(ui->newButton,
                                  &QPushButton::clicked, this,
                                  &InventorySlotEditor::onNewItem);
    }

    ui->listView->setModel(&model);
    ui->groupBox->setTitle((m_itemsOnly ? tr("Items") : tr("Blocks")) +
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

    ui->groupBox->setTitle((m_itemsOnly ? tr("Items") : tr("Blocks")) +
                           QString(" (%1)").arg(model.rowCount()));

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
    if (canAddItem(false)) {
        BlockItemSelectorDialog dialog(this, slot->selectCategory());

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
    }
    show();
}

void InventorySlotEditor::onNewItemTag() {
    if (canAddItem(true)) {
        TagSelectorDialog dialog(
            this, (m_itemsOnly) ? CodeFile::ItemTag : CodeFile::BlockTag);

        if (dialog.exec()) {
            InventoryItem invItem(dialog.getSelectedID());
            if ((!slot->getItems().contains(invItem)) && slot->getAcceptTag()) {
                slot->appendItem(invItem);
                if (!slot->getItems().contains(invItem)) return;

                appendItem(invItem);
            }
        }
    }
    show();
}

void InventorySlotEditor::checkRemove() {
    const auto indexes = ui->listView->selectionModel()->selectedIndexes();

    ui->removeButton->setDisabled(indexes.isEmpty());
}

bool InventorySlotEditor::canAddItem(const bool addingTag) {
    if (!(slot->acceptPolicies() & InventorySlot::AcceptTags)
        && (slot->acceptPolicies() & InventorySlot::AcceptItems)) {
        bool hasTags = false;
        for (int row = 0; row < model.rowCount(); ++row) {
            QStandardItem *item    = model.item(row);
            const auto   &&invItem =
                item->data(Qt::UserRole + 1).value<InventoryItem>();
            if (invItem.isTag()) {
                hasTags = true;
                break;
            }
        }
        if ((model.rowCount() > 0) && (hasTags || addingTag)) {
            QMessageBox::critical(
                this,
                tr("Cannot add new item"),
                tr(
                    "This slot only accepts either an item, multiple items or an item tag."),
                QMessageBox::Ok,
                QMessageBox::Ok);
            return false;
        }
    }
    return true;
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
