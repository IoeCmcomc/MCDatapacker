#include "mcrinvslot.h"

#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
#include "mainwindow.h"
#include "globalhelpers.h"
#include "mcrinvsloteditor.h"
#include "codefile.h"

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

MCRInvSlot::MCRInvSlot(QWidget *parent,
                       const MCRInvItem &item) : QFrame(parent) {
    setAcceptDrops(true);
    setMinimumSize(36, 36);
    setBackground();

    clearItems();
    if (!item.isEmpty())
        setItem(item);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &MCRInvSlot::customContextMenuRequested, this,
            &MCRInvSlot::onCustomContextMenu);
    connect(this, &MCRInvSlot::itemChanged, this, &MCRInvSlot::onItemChanged);
}

void MCRInvSlot::setBackground(const QString &color) {
    if (color.isEmpty()) {
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Sunken);
        setLineWidth(2);
    } else {
        auto disabledColor = palette().color(QPalette::Disabled,
                                             QPalette::Window);
        setStyleSheet(
            ".MCRInvSlot {"
            "   background: " + color + ";"
            "   border: 2px solid;"
            "   border-color: #373737 #FFF #FFF #373737;"
            "}"
            ".MCRInvSlot:disabled {"
            "background: " + disabledColor.name() + ";"
            "}"
            ".MCRInvSlot:hover {"
            "   background-color: hsla(0,0%,100%,.4);"
            "}"
            );
    }
}

void MCRInvSlot::setItem(const QVector<MCRInvItem> &items) {
    this->items = items;
    showItem();
    emit itemChanged();
}

void MCRInvSlot::setItem(const MCRInvItem &item) {
    clearItems();
    this->items.push_back(item);
    showItem();
    emit itemChanged();
}

void MCRInvSlot::appendItem(const MCRInvItem &item) {
    /*qDebug() << "slot appendItem" << item << items.contains(item); */
    if (items.contains(item)) return;

    items.push_back(item);
    emit itemChanged();
}

void MCRInvSlot::appendItems(const QVector<MCRInvItem> &items) {
    this->items.append(items);
}

void MCRInvSlot::insertItem(const int index, const MCRInvItem &item) {
    if (items.contains(item)) return;

    items.insert(index, item);
    emit itemChanged();
}

void MCRInvSlot::removeItem(const int index) {
    items.remove(index);
    emit itemChanged();
}

int MCRInvSlot::removeItem(const MCRInvItem &item) {
    int r = items.removeAll(item);

    emit itemChanged();

    return r;
}

void MCRInvSlot::clearItems() {
    if (items.isEmpty()) return;

    this->items.clear();
    showItem();
    emit itemChanged();
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

QString MCRInvSlot::toolTipText() {
    QString ret;

    if (items.count() > 1) {
        QStringList itemNames;
        int         c = 0;
        for (const auto &item : items) {
            itemNames.push_back(item.getName());
            if (c > 5) {
                itemNames.push_back(tr("..."));
                break;
            }
            ++c;
        }
        ret = QString(tr("%1 items: ") +
                      itemNames.join(QStringLiteral(", "))).arg(items.count());
    } else if (items.count() == 1) {
        ret = getItem().toolTip();
    } else {
        return QString();
    }
    return ret;
}

void MCRInvSlot::onCustomContextMenu(const QPoint &point) {
    auto *cMenu = new QMenu(this);

    if (items.count() != 0) {
        QAction *removeAction = new QAction(tr("Remove"), cMenu);
        connect(removeAction, &QAction::triggered, [ = ]() {
            clearItems();
        });
        cMenu->addAction(removeAction);
        /*qDebug() << items.count(); */
    }

    if (getAcceptTag()) {
        QAction *seclectTagAction = new QAction(tr("Select tag..."), cMenu);
        seclectTagAction->setEnabled(getAcceptTag());
        connect(seclectTagAction, &QAction::triggered, [ = ]() {
            TagSelectorDialog dialog(this, CodeFile::ItemTag);
            if (dialog.exec()) {
                setItem(MCRInvItem(dialog.getSelectedID()));
            }
        });
        cMenu->addAction(seclectTagAction);
    }

    auto *cMenu2 = new QMenu(this);
    cMenu2->addAction("Test");

    if (!cMenu->isEmpty()) {
        cMenu->exec(this->mapToGlobal(point));
    }
    delete cMenu;

    update();
}

void MCRInvSlot::onItemChanged() {
    setToolTip(toolTipText());
    if (!items.isEmpty()) {
        curItemIndex = 0;
        timer.start(1000, this);
    } else {
        timer.stop();
    }
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
            /*qDebug() << acceptMultiItems; */
            dialog.setAllowMultiItems(acceptMultiItems);
            if (dialog.exec()) {
                setItem(dialog.getSelectedItems());
            }
            break;
        }

        case Qt::MiddleButton: {
            if (getAcceptMultiItems()) {
                auto *editor = new MCRInvSlotEditor(this);
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

    if (isVisible()) {
        QPainter painter(this);
        if ((!items.isEmpty()) && (!itemHidden)) {
            /*qDebug() << "paintEvent" << curItemIndex << items.count(); */
            if ((curItemIndex < items.count()) && (curItemIndex != -1)) {
                auto pixmap = items[curItemIndex].getPixmap();
                if (pixmap.isNull()) return;

                painter.drawPixmap((width() - pixmap.width()) / 2,
                                   (height() - pixmap.height()) / 2,
                                   pixmap);
            }
        }
        painter.end();
    }
}

void MCRInvSlot::timerEvent(QTimerEvent *event) {
    /*
       qDebug() << "timerEvent" << event << curItemIndex;
       qDebug() << event->timerId() << timer.timerId();
     */
    if (event->timerId() != timer.timerId()) {
        QWidget::timerEvent(event);
        return;
    }
    ++curItemIndex;
    if (curItemIndex >= items.count())
        curItemIndex = 0;
    /*qDebug() << curItemIndex; */
    update();
    /*qDebug() << "itmerEvent end"; */
}

void MCRInvSlot::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-mcrinvitem")) {
        auto *source = qobject_cast<MCRInvSlot*>(event->source());
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
        auto *source = qobject_cast<MCRInvSlot*>(event->source());
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
            auto      *source   = qobject_cast<MCRInvSlot*>(event->source());
            QByteArray itemData = event->mimeData()->data(
                "application/x-mcrinvitem");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QVector<MCRInvItem> dropInvItems;
            dataStream >> dropInvItems;

            bool hasTag = false;
            for (const auto &item : dropInvItems)
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
                        setItem(dropInvItems);

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
            auto filepath =
                event->mimeData()->urls().at(0).toLocalFile();
            auto    dirpath = MainWindow::getCurDir();
            QString id      = Glhp::toNamespacedID(dirpath,
                                                   filepath);

            if (!id.isEmpty()) {
                MCRInvItem newItem(id);
                if (acceptTag || (!newItem.getIsTag())) {
                    if (event->keyboardModifiers() & Qt::ControlModifier)
                        appendItem(newItem);
                    else
                        setItem(newItem);

                    event->setDropAction(Qt::LinkAction);
                    event->accept();
                }
            }
        } else if (event->mimeData()->hasText()) {
            QString id = event->mimeData()->text();
            if (!id.isEmpty()) {
                MCRInvItem newItem(id);
                if (acceptTag || (!newItem.getIsTag())) {
                    setItem(newItem);

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

void MCRInvSlot::startDrag([[maybe_unused]] QMouseEvent *event) {
    if ((this->items.isEmpty()) || itemNamespacedID().isEmpty())
        return;

    QPoint offset(32 - 1, 32 - 1);
    offset /= 2;

    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << items;

    auto *mimeData = new QMimeData;
    mimeData->setData("application/x-mcrinvitem", itemData);
    mimeData->setText(items[0].getNamespacedID());

    auto *drag = new QDrag(this);
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
            clearItems();
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
