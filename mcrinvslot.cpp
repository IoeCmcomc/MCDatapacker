#include "mcrinvslot.h"

#include "blockitemselectordialog.h"

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QGridLayout>
#include <QApplication>
#include <QMenu>
#include <QAction>

MCRInvSlot::MCRInvSlot(QWidget *parent, MCRInvItem *item) : QFrame(parent) {
    setAcceptDrops(true);
    setMinimumSize(36, 36);
/*    setBackground(); */

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    setItem(item);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &MCRInvSlot::customContextMenuRequested, this,
            &MCRInvSlot::onCustomContextMenu);
}

void MCRInvSlot::setBackground(QString color) {
    if (color.isEmpty()) {
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Sunken);
        setLineWidth(2);
    } else {
        setStyleSheet(
            ".MCRInvSlot {"
            "   background: " + color + ";"
            "   border: 2px solid;"
            "   border-color: #373737 #FFF #FFF #373737;"
            "}"
            ".MCRInvSlot:hover {"
            "   background-color: hsla(0,0%,100%,.4);"
            "}");
    }
}

void MCRInvSlot::setItem(MCRInvItem *item, bool emitSignal) {
    if (item != nullptr) {
        if (item->getNamespacedID().isEmpty()) return;

        removeItem();
        this->item = item;
        layout()->addWidget(item);
        if (emitSignal) emit itemChanged();
    }
}

void MCRInvSlot::removeItem(bool emitSignal) {
    /*if(layout()->isEmpty() || layout()->count() == 0) return; */
    layout()->removeWidget(this->item);
    delete this->item;
    this->item = nullptr;
    if (emitSignal) emit itemChanged();
}

MCRInvItem* MCRInvSlot::getItem() {
    return this->item;
}

QString MCRInvSlot::itemNamespacedID() {
    return (item != nullptr) ? item->getNamespacedID() : "";
}

QString MCRInvSlot::itemName() {
    return (item != nullptr) ? item->getName() : "";
}

void MCRInvSlot::onCustomContextMenu(const QPoint &point) {
    QMenu *cMenu = new QMenu(this);

    if (item != nullptr) {
        QAction *removeAction = new QAction(tr("Remove"), cMenu);
        connect(removeAction, &QAction::triggered, [ = ]() {
            removeItem(true);
        });
        cMenu->addAction(removeAction);
    }

    if (!cMenu->isEmpty()) {
        cMenu->exec(this->mapToGlobal(point));
    }
    delete cMenu;
}

bool MCRInvSlot::getIsCreative() const {
    return isCreative;
}

void MCRInvSlot::setIsCreative(bool value) {
    isCreative = value;
}

void MCRInvSlot::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() ^ event->button()) {
        QFrame::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton)
        mousePressPos = event->pos();
    QFrame::mousePressEvent(event);
}

void MCRInvSlot::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - mousePressPos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    startDrag(event);
}

void MCRInvSlot::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && (!isDragged)
        && this->rect().contains(event->pos())) {
        /* Clicked properly */
        BlockItemSelectorDialog dialog(this);
        if (dialog.exec()) {
            setItem(new MCRInvItem(this, dialog.getSelectedID()));
        }
    }
    isDragged = false;
    QFrame::mouseReleaseEvent(event);
}

void MCRInvSlot::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-mcrinvitem")) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        } else if (event->source() != this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->setDropAction(Qt::IgnoreAction);
            event->ignore();
        }
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-mcrinvitem")) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        } else if (event->source() != this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->setDropAction(Qt::IgnoreAction);
            event->ignore();
        }
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-mcrinvitem")
        && event->mimeData()->hasText()) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if (!isCreative) {
/*
              QByteArray itemData = event->mimeData()->data("application/x-mcrinvitem");
              QDataStream dataStream(&itemData, QIODevice::ReadOnly);
 */

/*            QPoint offset; */
            QString id;
/*
              dataStream >> offset
                         >> id;
 */
            id = event->mimeData()->text();

            QString itemID;
            if (this->item) itemID = this->item->getNamespacedID();
            if (itemID != id) {
                if ((event->source() != this) && (!itemID.isEmpty())
                    && (!source->isCreative)) {
                    MCRInvItem *swapItem = new MCRInvItem(this, itemID);
                    source->setItem(swapItem);
                }
                MCRInvItem *nitem = new MCRInvItem(this, id);
                setItem(nitem, true);
            }
        }

        if (source->isCreative ||
            (event->keyboardModifiers() & Qt::ControlModifier)) {
            event->setDropAction(Qt::CopyAction);
            emit itemChanged();
            event->accept();
        } else if (event->source() != this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->setDropAction(Qt::IgnoreAction);
            event->ignore();
        }
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::startDrag(QMouseEvent *event) {
    if (!this->item || this->item->getNamespacedID().isEmpty())
        return;

    QPoint      offset(event->pos() - this->item->pos());
    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << offset
               << item->getNamespacedID();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-mcrinvitem", itemData);
    mimeData->setText(item->getNamespacedID());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(*this->item->pixmap());
    drag->setHotSpot(offset);

    if (!(isCreative || (qApp->keyboardModifiers() & Qt::ControlModifier))) {
        this->item->hide();
    }

    MCRInvItem *before = this->item;

    int dragActions = drag->exec(
        Qt::CopyAction | Qt::MoveAction | Qt::IgnoreAction,
        Qt::IgnoreAction);
    isDragged = true;
    if (dragActions == Qt::CopyAction) {
        /*qDebug() << "Dragged and dropped for copying (Qt::CopyAction)"; */
    } else if (dragActions == Qt::IgnoreAction) {
        /*qDebug() << "Dragged and dropped for ignoring (Qt::IgnoreAction)"; */
        this->item->show();
    } else if (dragActions == Qt::MoveAction) {
        /*qDebug() << "Dragged and dropped for moving (Qt::MoveAction)"; */
        if (this->item == before) {
            removeItem(true);
        } else {
            emit itemChanged();
        }
    } else {
    }
}
