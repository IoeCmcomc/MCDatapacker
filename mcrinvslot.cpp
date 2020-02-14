#include "mcrinvslot.h"

#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QGridLayout>
#include <QApplication>

MCRInvSlot::MCRInvSlot(QWidget *parent, MCRInvItem *item) : QFrame(parent)
{
    setAcceptDrops(true);
    setMinimumSize(36, 36);
    setBackground();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    setItem(item);
}

void MCRInvSlot::setBackground(QString color) {
    if(color.isEmpty()) {
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Sunken);
        setLineWidth(2);
    } else {
        setStyleSheet(
            ".MCRInvSlot {"
            "   background: "+color+";"
            "   border: 2px solid;"
            "   border-color: #373737 #FFF #FFF #373737;"
            "}"
            ".MCRInvSlot:hover {"
            "   background-color: hsla(0,0%,100%,.4);"
            "}");
    }
}

void MCRInvSlot::setItem(MCRInvItem *item, bool emitSignal) {
    if(item != nullptr) {
        if(item->namespacedID.isEmpty()) return;
        removeItem();
        this->item = item;
        layout()->addWidget(item);
        if(emitSignal) emit itemChanged();
    }
}

void MCRInvSlot::removeItem(bool emitSignal) {
    //if(layout()->isEmpty() || layout()->count() == 0) return;
    layout()->removeWidget(this->item);
    delete this->item;
    this->item = nullptr;
    if(emitSignal) emit itemChanged();
}

MCRInvItem* MCRInvSlot::getItem() {
    return this->item;
}

QString MCRInvSlot::itemNamespacedID() {
    return (item != nullptr) ? item->namespacedID : "";
}

QString MCRInvSlot::itemName() {
    return (item != nullptr) ? item->getName() : "";
}

void MCRInvSlot::dragEnterEvent(QDragEnterEvent *event)
{
    //qDebug() << "dragEnterEvent";
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        } else if(event->source() != this) {
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

void MCRInvSlot::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "dragMoveEvent";
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
        } else if(event->source() != this) {
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

void MCRInvSlot::dropEvent(QDropEvent *event)
{
    //qDebug() << "dropEvent" << isCreative;
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());
        if(!isCreative) {
            QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QPoint offset;
            QString id;
            dataStream >> offset
                       >> id;

            QString itemID;
            if(this->item) itemID = this->item->namespacedID;
            if(itemID != id) {
                if ((event->source() != this) && (!itemID.isEmpty())
                                              && (!source->isCreative)) {
                    MCRInvItem *swapItem = new MCRInvItem(this, itemID);
                    source->setItem(swapItem);
                }
                MCRInvItem *nitem = new MCRInvItem(this, id);
                setItem(nitem, true);
            }
        }

        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
            emit itemChanged();
            event->accept();
        } else if(event->source() != this) {
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

void MCRInvSlot::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "mousePressEvent";

    if (!this->item || this->item->namespacedID.isEmpty())
        return;

    QPoint offset(event->pos() - this->item->pos());
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << offset
               << item->namespacedID;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(*this->item->pixmap());
    drag->setHotSpot(offset);

    if(!isCreative) {
        this->item->hide();
    }

    MCRInvItem *before = this->item;

    int dragActions = drag->exec(Qt::CopyAction | Qt::MoveAction | Qt::IgnoreAction, Qt::IgnoreAction);

//    qDebug() << "Is copy action:" << (dragActions == Qt::CopyAction);
//    qDebug() << "Is move action:" << (dragActions == Qt::MoveAction);
//    qDebug() << "Is ignore action:" << (dragActions == Qt::IgnoreAction);
//    qDebug() << "Action value:" << dragActions;

    if(dragActions == Qt::CopyAction) {
        //qDebug() << "Dragged and dropped for copying (Qt::CopyAction)";
    } else if (dragActions == Qt::IgnoreAction) {
        //qDebug() << "Dragged and dropped for ignoring (Qt::IgnoreAction)";
        this->item->show();
    } else if(dragActions == Qt::MoveAction) {
        //qDebug() << "Dragged and dropped for moving (Qt::MoveAction)";
        if(this->item == before) {
            removeItem(true);
        } else {
            emit itemChanged();
        }
    } else {}
}
