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
#include <QVariant>

MCRInvSlot::MCRInvSlot(QWidget *parent, MCRInvItem item) : QFrame(parent) {
    setAcceptDrops(true);
    setMinimumSize(36, 36);
    setBackground();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
    removeItem();
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

void MCRInvSlot::setItem(MCRInvItem item, bool emitSignal) {
    this->item = item;
    showItem();
    setToolTip(item.getName());
    if (emitSignal) emit itemChanged();
}

void MCRInvSlot::removeItem(bool emitSignal) {
    this->item = MCRInvItem();
    showItem();
    setToolTip(QString());
    if (emitSignal) emit itemChanged();
}

MCRInvItem MCRInvSlot::getItem() const {
    return this->item;
}

QString MCRInvSlot::itemNamespacedID() {
    return item.getNamespacedID();
}

QString MCRInvSlot::itemName() {
    return item.getName();
}

void MCRInvSlot::onCustomContextMenu(const QPoint &point) {
    QMenu *cMenu = new QMenu(this);

    if (!item.isEmpty()) {
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
            removeItem();
            setItem(MCRInvItem(dialog.getSelectedID(), this));
        }
    }
    isDragged = false;
    QFrame::mouseReleaseEvent(event);
    update();
}

void MCRInvSlot::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    if ((!item.isEmpty()) && (!item.getPixmap().isNull()) && (!itemHidden))
        painter.drawPixmap(this->rect(), item.getPixmap());
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
    } else if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
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
    } else if (event->mimeData()->hasText()) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::dropEvent(QDropEvent *event) {
    if (!isCreative) {
        if (event->mimeData()->hasFormat("application/x-mcrinvitem")) {
            MCRInvSlot *source = qobject_cast<MCRInvSlot*>(event->source());

            QByteArray itemData = event->mimeData()->data(
                "application/x-mcrinvitem");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QVariant vari;

            dataStream >> vari;

            MCRInvItem dropInvItem = vari.value<MCRInvItem>();

            if (!item.isEmpty()) {
                if ((event->source() != this) && (!source->isCreative)) {
                    /*source->setItem(item); */
                    MCRInvItem swapItem(item.getNamespacedID(), this);
                    source->setItem(swapItem);
                }
            }
            if (!(item == dropInvItem)) {
                setItem(dropInvItem, true);
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
                delete &dropInvItem;
            }
        } else if (event->mimeData()->hasText()) {
            QString id = event->mimeData()->text();
            if (!id.isEmpty()) {
                MCRInvItem newItem(id, this);
                setItem(newItem, true);

                event->setDropAction(Qt::CopyAction);
                emit itemChanged();
                event->accept();
            }
        }
        update();
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::startDrag(QMouseEvent *event) {
    if ((this->item.isEmpty()) || this->item.getNamespacedID().isEmpty())
        return;

    QPoint offset(32 - 1, 32 - 1);
    offset /= 2;
    QVariant vari;
    vari.setValue(item);

    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << vari;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-mcrinvitem", itemData);
    mimeData->setText(item.getNamespacedID());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(this->item.getPixmap());
    drag->setHotSpot(offset);

    if (!(isCreative || (qApp->keyboardModifiers() & Qt::ControlModifier))) {
        this->hideItem();
    }

    MCRInvItem before = this->item;

    int dragActions = drag->exec(
        Qt::CopyAction | Qt::MoveAction | Qt::IgnoreAction,
        Qt::IgnoreAction);
    isDragged = true;
    /*qDebug() << "Before" << before.getNamespacedID() << "After" << */
    item.getNamespacedID();
    if (dragActions == Qt::CopyAction) {
        /*qDebug() << "Dragged and dropped for copying (Qt::CopyAction)"; */
        this->showItem();
    } else if (dragActions == Qt::IgnoreAction) {
        /*qDebug() << "Dragged and dropped for ignoring (Qt::IgnoreAction)"; */
        this->showItem();
    } else if (dragActions == Qt::MoveAction) {
        /*qDebug() << "Dragged and dropped for moving (Qt::MoveAction)"; */
        if (this->item == before) {
            removeItem(true);
        } else {
            showItem();
            emit itemChanged();
        }
    } else {
    }
    update();
}

void MCRInvSlot::hideItem() {
    itemHidden = true;
    update();
}

void MCRInvSlot::showItem() {
    itemHidden = false;
    update();
}
