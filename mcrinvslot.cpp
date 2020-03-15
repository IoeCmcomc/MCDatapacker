#include "mcrinvslot.h"

#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
#include "mcrinvsloteditor.h"
#include "mainwindow.h"
#include "globalhelpers.h"

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
    clearItems();
    if (!item.isEmpty())
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

void MCRInvSlot::setItem(const QVector<MCRInvItem> &items, bool emitSignal) {
    this->items = items;
    showItem();
    setToolTip(toolTipText());
    if (emitSignal) emit itemChanged();
}

void MCRInvSlot::setItem(MCRInvItem item, bool emitSignal) {
    clearItems();
    this->items.push_back(item);
    showItem();
    setToolTip(toolTipText());
    if (emitSignal) emit itemChanged();
}

void MCRInvSlot::appendItem(MCRInvItem item, bool emitSignal) {
    if (items.contains(item)) return;

    items.push_back(item);

    setToolTip(toolTipText());
    if (emitSignal) emit itemChanged();
}

void MCRInvSlot::insertItem(const int index, MCRInvItem item, bool emitSignal) {
    if (items.contains(item)) return;

    items.insert(index, item);

    setToolTip(toolTipText());
    if (emitSignal) emit itemChanged();
}

void MCRInvSlot::removeItem(const int index) {
    items.remove(index);

    setToolTip(toolTipText());
}

QString MCRInvSlot::toolTipText() {
    if (items.count() > 1) {
        QStringList itemNames;
        for (auto item : items) {
            itemNames.push_back(item.getName());
        }
        return QString(tr("%1 items: ") +
                       itemNames.join(QStringLiteral(", "))).arg(items.count());
    } else if (items.count() == 1) {
        return itemName();
    } else {
        return QString();
    }
}

int MCRInvSlot::removeItem(const MCRInvItem item) {
    int r = items.removeAll(item);

    setToolTip(toolTipText());
    return r;
}

void MCRInvSlot::clearItems(bool emitSignal) {
    if (items.isEmpty()) return;

    this->items.clear();
    showItem();
    setToolTip(QString());
    if (emitSignal) emit itemChanged();
}

MCRInvItem &MCRInvSlot::getItem(const int index) {
    return this->items[index];
}

QVector<MCRInvItem> &MCRInvSlot::getItems() {
    return this->items;
}

QString MCRInvSlot::itemNamespacedID(const int index) {
    if (items.isEmpty()) return QString();

    return items[index].getNamespacedID();
}

QString MCRInvSlot::itemName(const int index) {
    if (items.isEmpty()) return QString();

    return items[index].getName();
}

void MCRInvSlot::onCustomContextMenu(const QPoint &point) {
    QMenu *cMenu = new QMenu(this);

    if (items.count() != 0) {
        QAction *removeAction = new QAction(tr("Remove"), cMenu);
        connect(removeAction, &QAction::triggered, [ = ]() {
            clearItems(true);
        });
        cMenu->addAction(removeAction);
        qDebug() << items.count();
    }

    if (getAcceptTag()) {
        QAction *seclectTagAction = new QAction(tr("Select tag..."), cMenu);
        seclectTagAction->setEnabled(getAcceptTag());
        connect(seclectTagAction, &QAction::triggered, [ = ]() {
            TagSelectorDialog dialog(this);
            if (dialog.exec()) {
                setItem(MCRInvItem(dialog.getSelectedID()));
            }
        });
        cMenu->addAction(seclectTagAction);
    }

    QMenu *cMenu2 = new QMenu(this);
    cMenu2->addAction("Test");

    if (!cMenu->isEmpty()) {
        cMenu->exec(this->mapToGlobal(point));
    }
    delete cMenu;

    update();
}

bool MCRInvSlot::getAcceptMultiItems() const {
    return acceptMultiItems;
}

void MCRInvSlot::setAcceptMultiItems(bool value) {
    acceptMultiItems = value;
}

bool MCRInvSlot::getAcceptTag() const {
    return acceptTag;
}

void MCRInvSlot::setAcceptTag(bool value) {
    acceptTag = value;
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
    if ((!isDragged) && this->rect().contains(event->pos())) {
        /* Clicked properly */
        switch (event->button()) {
        case Qt::LeftButton: {
            BlockItemSelectorDialog dialog(this);
            if (dialog.exec()) {
                setItem(MCRInvItem(dialog.getSelectedID()));
            }
            break;
        }

        case Qt::MiddleButton: {
            if (getAcceptMultiItems()) {
                MCRInvSlotEditor *editor =
                    new MCRInvSlotEditor(this, mapToGlobal(event->pos()));
                editor->show();
                update();
            }
            break;
        }

        default: {
            break;
        }
        }
    }
    isDragged = false;
    QFrame::mouseReleaseEvent(event);
    update();
}

void MCRInvSlot::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    if ((!items.isEmpty()) && (!items[0].getPixmap().isNull()) &&
        (!itemHidden)) {
        auto pixmap = items[0].getPixmap();
        painter.drawPixmap((width() - pixmap.width()) / 2,
                           (height() - pixmap.height()) / 2,
                           pixmap);
    }
    painter.end();
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
    } else if (event->mimeData()->hasText() ||
               event->mimeData()->hasFormat("text/uri-list")) {
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
            MCRInvSlot *source   = qobject_cast<MCRInvSlot*>(event->source());
            QByteArray  itemData = event->mimeData()->data(
                "application/x-mcrinvitem");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

/*
              QVariant vari;
              dataStream >> vari;
              MCRInvItem dropInvItem = vari.value<MCRInvItem>();
 */

            QVector<MCRInvItem> dropInvItems;
            dataStream >> dropInvItems;

            bool hasTag = false;
            for (auto item : dropInvItems)
                if (item.getIsTag()) {
                    hasTag = true;
                    break;
                }

            if ((!dropInvItems.isEmpty()) && (items != dropInvItems)) {
                if ((getAcceptTag() && source->getAcceptTag())
                    || (!hasTag)) {
                    if (getAcceptMultiItems() || (dropInvItems.count() == 1)) {
                        if ((event->source() != this) &&
                            (!source->isCreative) &&
                            (!items.isEmpty())) {
                            /*source->setItem(item); */
                            MCRInvItem swapItem(items[0].getNamespacedID());
                            source->setItem(swapItem);
                        }
                        setItem(dropInvItems, true);

                        if (source->isCreative ||
                            (event->keyboardModifiers() &
                             Qt::ControlModifier)) {
                            event->setDropAction(Qt::CopyAction);
                            event->accept();
                            return;
                        } else if (event->source() != this) {
                            event->setDropAction(Qt::MoveAction);
                            event->accept();
                        }
                    }
                }
            }
        } else if (event->mimeData()->hasFormat("text/uri-list")) {
            auto    filepath = event->mimeData()->urls().at(0).toLocalFile();
            auto    dirpath  = qobject_cast<MainWindow*>(window())->getCurDir();
            QString id       = GlobalHelpers::toNamespacedID(dirpath, filepath);

            if (!id.isEmpty()) {
                MCRInvItem newItem(id);
                if (acceptTag || (!newItem.getIsTag())) {
                    setItem(newItem, true);

                    event->setDropAction(Qt::CopyAction);
                    event->accept();
                }
            }
        } else if (event->mimeData()->hasText()) {
            QString id = event->mimeData()->text();
            if (!id.isEmpty()) {
                MCRInvItem newItem(id);
                if (acceptTag || (!newItem.getIsTag())) {
                    setItem(newItem, true);

                    event->setDropAction(Qt::CopyAction);
                    event->accept();
                }
            }
        }
    }
    if (event->isAccepted()) {
        update();
        event->accept();
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void MCRInvSlot::startDrag(QMouseEvent *event) {
    if ((this->items.isEmpty()) || itemNamespacedID().isEmpty())
        return;

    QPoint offset(32 - 1, 32 - 1);
    offset /= 2;
/*
      QVariant vari;
      vari.setValue(items[0]);
 */

    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
/*    dataStream << vari; */
    dataStream << items;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-mcrinvitem", itemData);
    mimeData->setText(items[0].getNamespacedID());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(this->items[0].getPixmap());
    drag->setHotSpot(offset);

    if (!(isCreative || (qApp->keyboardModifiers() & Qt::ControlModifier))) {
        this->hideItem();
    }

    QVector<MCRInvItem> before = this->items;

    int dragActions = drag->exec(
        Qt::CopyAction | Qt::MoveAction | Qt::IgnoreAction,
        Qt::IgnoreAction);
    isDragged = true;
    /*qDebug() << "Before" << before.getNamespacedID() << "After" << */
    items[0].getNamespacedID();
    if (dragActions == Qt::CopyAction) {
        /*qDebug() << "Dragged and dropped for copying (Qt::CopyAction)"; */
        this->showItem();
        update();
    } else if (dragActions == Qt::IgnoreAction) {
        /*qDebug() << "Dragged and dropped for ignoring (Qt::IgnoreAction)"; */
        this->showItem();
    } else if (dragActions == Qt::MoveAction) {
        /*qDebug() << "Dragged and dropped for moving (Qt::MoveAction)"; */
        if (this->items == before) {
            clearItems(true);
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
