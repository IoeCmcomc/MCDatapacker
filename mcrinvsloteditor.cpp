#include "mcrinvsloteditor.h"
#include "ui_mcrinvsloteditor.h"

#include "blockitemselectordialog.h"
#include "tagselectordialog.h"

#include <QStandardItem>
#include <QPushButton>
#include <QMenu>
#include <QWidgetAction>
#include <QScreen>
/*#include <QItemSelectionModel> */
#include <QMouseEvent>

MCRInvSlotEditor::MCRInvSlotEditor(MCRInvSlot *parent, QPoint pos) :
    QFrame(parent),
    ui(new Ui::MCRInvSlotEditor) {
    /*qDebug() << "MCRInvSlotEditor::MCRInvSlotEditor"; */
    ui->setupUi(this);
    this->slot = parent;
    this->pos  = pos;

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

    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MCRInvSlotEditor::checkRemove);
    connect(ui->removeButton,
            &QPushButton::clicked,
            this,
            &MCRInvSlotEditor::onRemoveItem);

    adjustSize();

    auto   scrSize  = qApp->screens()[0]->size();
    QPoint newPoint = mapToGlobal(pos);

    if ((newPoint.x() + width()) > scrSize.width())
        newPoint.rx() = scrSize.width() - width();
    if ((newPoint.y() + height()) > scrSize.height())
        newPoint.ry() = scrSize.height() - height();

    newPoint.rx() = qMax(newPoint.x(), 0);
    newPoint.ry() = qMax(newPoint.y(), 0);

    move(newPoint);
    checkRemove();
}

MCRInvSlotEditor::~MCRInvSlotEditor() {
/*    qDebug() << this << "is going to be deleted."; */
    delete ui;
}

void MCRInvSlotEditor::mousePressEvent(QMouseEvent *event) {
    /*qDebug() << "mousePressEvent" << event; */
    if ((!rect().contains(event->pos()))
        || (event->button() != Qt::LeftButton)) {
        deleteLater();
        slot->update();
    }
    QFrame::mousePressEvent(event);
}

void MCRInvSlotEditor::onNewItem() {
    BlockItemSelectorDialog dialog(this);

    if (dialog.exec()) {
        auto invItem = MCRInvItem(dialog.getSelectedID());
        if (!slot->getItems().contains(invItem)) {
            slot->appendItem(invItem);
            if (!slot->getItems().contains(invItem)) return;

            appendItem(invItem);
        }
    }
    show();
}

void MCRInvSlotEditor::onNewItemTag() {
    TagSelectorDialog dialog(this);

    if (dialog.exec()) {
        auto invItem = MCRInvItem(dialog.getSelectedID());
        if ((!slot->getItems().contains(invItem)) && slot->getAcceptTag()) {
            slot->appendItem(invItem);
            if (!slot->getItems().contains(invItem)) return;

            appendItem(invItem);
        }
    }
    show();
}

void MCRInvSlotEditor::checkRemove() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    ui->removeButton->setDisabled(indexes.isEmpty());
}

void MCRInvSlotEditor::onRemoveItem() {
    auto indexes = ui->listView->selectionModel()->selectedIndexes();

    if (indexes.isEmpty()) return;

    QStandardItem *item    = model.itemFromIndex(indexes[0]);
    auto           i       = model.indexFromItem(item).row();
    MCRInvItem     invItem = item->data(Qt::UserRole + 1).value<MCRInvItem>();

    if (!slot->getItems().contains(invItem)) return;

    slot->removeItem(invItem);
    if (slot->getItems().contains(invItem)) return;

    model.removeRow(i);
}

void MCRInvSlotEditor::appendItem(const MCRInvItem &invItem) {
    QStandardItem *item = new QStandardItem();

    item->setIcon(QIcon(invItem.getPixmap()));
    item->setSizeHint(QSize(32 + (3 * 2), 32 + (3 * 2)));
    QVariant vari;
    vari.setValue(invItem);
    item->setData(vari, Qt::UserRole + 1);
    item->setToolTip(invItem.getName());
    model.appendRow(item);
}
