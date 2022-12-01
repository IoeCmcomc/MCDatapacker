#include "inventoryslot.h"

#include "blockitemselectordialog.h"
#include "tagselectordialog.h"
#include "mainwindow.h"
#include "globalhelpers.h"
#include "inventorysloteditor.h"
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


static char _initTimer(QTimer &timer) {
    timer.setTimerType(Qt::VeryCoarseTimer);
    timer.setSingleShot(false);
    timer.setInterval(1000);
    return 0;
}

QTimer InventorySlot::m_timer = QTimer(qApp);

[[maybe_unused]] const char _ = _initTimer(InventorySlot::m_timer);


InventorySlot::InventorySlot(QWidget *parent, const InventoryItem &item)
    : QFrame(parent) {
    setAcceptDrops(true);
    /*setMinimumSize(sizeHint()); */
    adjustSize();
    setBackground();
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(2);

    clearItems();
    if (!item.isEmpty())
        setItem(item);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &InventorySlot::customContextMenuRequested, this,
            &InventorySlot::onCustomContextMenu);
    connect(this,
            &InventorySlot::itemChanged,
            this,
            &InventorySlot::onItemChanged);
    connect(&InventorySlot::m_timer, &QTimer::timeout, this,
            &InventorySlot::onTimerTimeout);

    static bool timerStarted = false;
    if (!timerStarted) {
        InventorySlot::m_timer.start();
        timerStarted = true;
    }
}

QSize InventorySlot::sizeHint() const {
    return QSize(36, 36);
}

void InventorySlot::setBackground(const QString &color) {
    if (color.isEmpty()) {
        setFrameShape(QFrame::StyledPanel);
        setFrameShadow(QFrame::Sunken);
        setLineWidth(2);
    } else {
        auto disabledColor = palette().color(QPalette::Disabled,
                                             QPalette::Window);
        setStyleSheet(
            ".InventorySlot {"
            "   background: " + color + ";"
            "   border: 2px solid;"
            "   border-color: #373737 #FFF #FFF #373737;"
            "}"
            ".InventorySlot:disabled {"
            "background: " + disabledColor.name() + ";"
            "}"
            ".InventorySlot:hover {"
            "   background-color: hsla(0,0%,70%,.4);"
            "}"
            );
    }
}

void InventorySlot::setItems(const QVector<InventoryItem> &items) {
    if (checkAcceptableItems(items)) {
        this->items = items;
        showItem();
        emit itemChanged();
    }
}

void InventorySlot:: setItem(const InventoryItem &item) {
    if (checkAcceptableItem(item)) {
        clearItems();
        this->items.push_back(item);
        showItem();
        emit itemChanged();
    }
}

void InventorySlot::appendItem(const InventoryItem &item) {
    if (!checkAcceptableItem(item) || items.contains(item))
        return;

    items.push_back(item);
    emit itemChanged();
}

void InventorySlot::appendItems(const QVector<InventoryItem> &items) {
    if (checkAcceptableItems(items)) {
        this->items.append(items);
        emit itemChanged();
    }
}

void InventorySlot::insertItem(const int index, const InventoryItem &item) {
    if (!checkAcceptableItem(item) || items.contains(item))
        return;

    items.insert(index, item);
    emit itemChanged();
}

void InventorySlot::removeItem(const int index) {
    items.remove(index);
    emit itemChanged();
}

int InventorySlot::removeItem(const InventoryItem &item) {
    int r = items.removeAll(item);

    emit itemChanged();

    return r;
}

void InventorySlot::clearItems() {
    if (items.isEmpty()) return;

    this->items.clear();
    showItem();
    emit itemChanged();
}

InventoryItem &InventorySlot::getItem(const int index) {
    return this->items[index];
}

QVector<InventoryItem> &InventorySlot::getItems() {
    return this->items;
}

QString InventorySlot::itemNamespacedID(const int index) {
    if (items.isEmpty()) return QString();

    return items[index].getNamespacedID();
}

QString InventorySlot::itemName(const int index) {
    if (items.isEmpty()) return QString();

    return items[index].getName();
}

QString InventorySlot::toolTipText() {
    QString ret;

    if (items.count() > 1) {
        QStringList itemNames;
        int         c = 0;
        for (const auto &item : qAsConst(items)) {
            itemNames.push_back(item.getName());
            if (c > 5) {
                itemNames.push_back("...");
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

bool InventorySlot::checkAcceptableItem(const InventoryItem &item) {
    if ((!getAcceptTag() && item.isTag())
        || (((m_selectCategory == SelectCategory::ObtainableItems) &&
             !item.isItem())
            || ((m_selectCategory == SelectCategory::Blocks) &&
                !item.isBlock()))) {
        return false;
    } else {
        return true;
    }
}

bool InventorySlot::checkAcceptableItems(const QVector<InventoryItem> &items) {
    static const auto onlyBlocksPred = [](const InventoryItem &item) {
                                           return !(!item.isBlock() &&
                                                    item.isItem());
                                       };
    static const auto hasTagPred = [](const InventoryItem &item) {
                                       return item.isTag();
                                   };
    static const auto hasUnobtainablePred = [](const InventoryItem &item) {
                                                return !item.isItem();
                                            };
    bool onlyBlocks =
        std::all_of(items.constBegin(), items.constEnd(), onlyBlocksPred);
    bool hasTag = std::any_of(items.constBegin(), items.constEnd(),
                              hasTagPred);
    bool hasUnobtainable = std::any_of(items.constBegin(), items.constEnd(),
                                       hasUnobtainablePred);

    if ((!getAcceptMultiple() && (items.size() > 1))
        || (!getAcceptTag() && hasTag)
        || ((m_selectCategory == SelectCategory::ObtainableItems) &&
            hasUnobtainable)
        || ((m_selectCategory == SelectCategory::Blocks) &&
            !onlyBlocks)) {
        return false;
    } else {
        return true;
    }
}

void InventorySlot::onCustomContextMenu(const QPoint &point) {
    auto *cMenu = new QMenu(this);

    if (items.count() != 0) {
        QAction *removeAction = new QAction(tr("Remove"), cMenu);
        connect(removeAction, &QAction::triggered, [this]() {
            clearItems();
        });
        cMenu->addAction(removeAction);
    }

    if (getAcceptTag()) {
        QAction *seclectTagAction = new QAction(tr("Select tag..."), cMenu);
        seclectTagAction->setEnabled(getAcceptTag());
        connect(seclectTagAction, &QAction::triggered, [this]() {
            TagSelectorDialog dialog(this, CodeFile::ItemTag);
            if (dialog.exec()) {
                setItem(InventoryItem(dialog.getSelectedID()));
            }
        });
        cMenu->addAction(seclectTagAction);
    }

    if (!cMenu->isEmpty()) {
        cMenu->exec(this->mapToGlobal(point));
    }
    cMenu->deleteLater();

    update();
}

void InventorySlot::onItemChanged() {
    setToolTip(toolTipText());
    curItemIndex = 0;
}

void InventorySlot::onTimerTimeout() {
    ++curItemIndex;
    if (curItemIndex >= items.count())
        curItemIndex = 0;
    update();
}

void InventorySlot::setSelectCategory(const SelectCategory &selectCategory) {
    m_selectCategory = selectCategory;
}

bool InventorySlot::getAcceptMultiple() const {
    return m_acceptPolicies.testFlag(AcceptMultiple);
}

void InventorySlot::setAcceptMultiple(bool value) {
    m_acceptPolicies.setFlag(AcceptMultiple, value);
}

bool InventorySlot::getAcceptTag() const {
    return m_acceptPolicies.testFlag(AcceptTags);
}

void InventorySlot::setAcceptTag(bool value) {
    m_acceptPolicies.setFlag(AcceptTags, value);
}

bool InventorySlot::getIsCreative() const {
    return isCreative;
}

void InventorySlot::setIsCreative(bool value) {
    isCreative = value;
}

void InventorySlot::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() ^ event->button()) {
        QFrame::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton)
        mousePressPos = event->pos();
    QFrame::mousePressEvent(event);
}

void InventorySlot::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - mousePressPos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    startDrag(event);
}

void InventorySlot::mouseReleaseEvent(QMouseEvent *event) {
    if ((!isDragged) && this->rect().contains(event->pos())) {
        /* Clicked properly */
        switch (event->button()) {
        case Qt::LeftButton: {
            BlockItemSelectorDialog dialog(this, m_selectCategory);
            dialog.setAllowMultiple(getAcceptMultiple());
            if (dialog.exec()) {
                setItems(dialog.getSelectedItems());
            }
            break;
        }

        case Qt::MiddleButton: {
            if (getAcceptMultiple()) {
                auto *editor = new InventorySlotEditor(this);
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

void InventorySlot::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);

    if (isVisible()) {
        QPainter painter(this);
        if ((!items.isEmpty()) && (!itemHidden)) {
            /*qDebug() << "paintEvent" << curItemIndex << items.count(); */
            if ((curItemIndex < items.count()) && (curItemIndex != -1)) {
                auto pixmap = items[curItemIndex].getPixmap();
                if (pixmap.isNull()) return;

                if (!isEnabled()) {
                    auto &&image = pixmap.toImage();

                    QRgb *st = (QRgb*) image.bits(); // Detach the image
                    const quint64 pixelCount = image.width() * image.height();

                    for (quint64 p = 0; p < pixelCount; ++p) {
                        const auto pixel = st[p];
                        int gray = qGray(pixel);
                        int alpha = qAlpha(pixel);
                        st[p] = qRgba(gray, gray, gray, alpha);
                    }

                    pixmap = QPixmap::fromImage(image);
                }
                painter.drawPixmap((width() - pixmap.width()) / 2,
                                   (height() - pixmap.height()) / 2,
                                   pixmap);
            }
        }
        painter.end();
    }
}

void InventorySlot::dragEnterEvent(QDragEnterEvent *event) {
    if ((event->source() == this) || isCreative) {
        event->ignore();
        return;
    }

    if (event->mimeData()->hasFormat("application/x-inventoryitem")) {
        const auto *source = qobject_cast<InventorySlot*>(event->source());

        if (source->isCreative) {
            event->setDropAction(Qt::CopyAction);
        }
        QByteArray &&itemData = event->mimeData()->data(
            QStringLiteral("application/x-inventoryitem"));
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QVector<InventoryItem> items;
        dataStream >> items;

        if (checkAcceptableItems(items)) {
            event->accept();
        } else {
            event->ignore();
        }
    } else if (event->mimeData()->hasText() ||
               event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        /*event->setDropAction(Qt::IgnoreAction); */
        event->ignore();
    }
}

void InventorySlot::dragMoveEvent(QDragMoveEvent *event) {
    update();
    QWidget::dragMoveEvent(event);
}

void InventorySlot::dragLeaveEvent(QDragLeaveEvent *event) {
    update();
    QWidget::dragLeaveEvent(event);
}

void InventorySlot::dropEvent(QDropEvent *event) {
    if (!isCreative) {
        if (event->mimeData()->hasFormat("application/x-inventoryitem")) {
            auto      *source   = qobject_cast<InventorySlot*>(event->source());
            QByteArray itemData = event->mimeData()->data(
                "application/x-inventoryitem");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            QVector<InventoryItem> dropInvItems;
            dataStream >> dropInvItems;


            if (items != dropInvItems) {
                if (!source->isCreative && (!items.isEmpty())) {
                    source->setItems(items);
                }
                setItems(dropInvItems);

                if (source->isCreative || (event->keyboardModifiers() &
                                           Qt::ControlModifier)) {
                    event->setDropAction(Qt::CopyAction);
                    event->accept();
                    return;
                } else if (event->source() != this) {
                    event->setDropAction(Qt::MoveAction);
                    event->accept();
                }
            }
        } else if (event->mimeData()->hasFormat("text/uri-list")) {
            auto filepath =
                event->mimeData()->urls().at(0).toLocalFile();
            QString id = Glhp::toNamespacedID(QDir::currentPath(),
                                              filepath);

            if (!id.isEmpty()) {
                InventoryItem newItem(id);
                if (getAcceptTag() || (!newItem.isTag())) {
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
                InventoryItem newItem(id);
                if (getAcceptTag() || (!newItem.isTag())) {
                    setItem(newItem);

                    event->setDropAction(Qt::LinkAction);
                    event->accept();
                }
            }
        }
    }
    if (event->isAccepted()) {
        update();
    } else {
        event->setDropAction(Qt::IgnoreAction);
        event->ignore();
    }
}

void InventorySlot::startDrag([[maybe_unused]] QMouseEvent *event) {
    if ((items.isEmpty()) || itemNamespacedID().isEmpty())
        return;

    QPoint offset(32 - 1, 32 - 1);
    offset /= 2;

    QByteArray  itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << items;

    auto *mimeData = new QMimeData;
    mimeData->setData("application/x-inventoryitem", itemData);
    mimeData->setText(items[0].getNamespacedID());

    auto *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(this->items[0].getPixmap());
    drag->setHotSpot(offset);

    if (!(isCreative || (qApp->keyboardModifiers() & Qt::ControlModifier))) {
        this->hideItem();
    }

    QVector<InventoryItem> before = this->items;

    int dragAction = drag->exec(
        Qt::CopyAction | Qt::MoveAction | Qt::IgnoreAction,
        Qt::MoveAction);
    isDragged = true;
    items[0].getNamespacedID();
    if (dragAction == Qt::MoveAction) {
        if (this->items == before) {
            clearItems();
        } else {
            emit itemChanged();
        }
    }
    showItem();
}

void InventorySlot::hideItem() {
    itemHidden = true;
    update();
}

void InventorySlot::showItem() {
    itemHidden = false;
    update();
}
