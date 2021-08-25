#include "inventoryitem.h"

#include "inventoryslot.h"
#include "mainwindow.h"

#include "globalhelpers.h"

#include <QPainter>
#include <QApplication>
#include <QGraphicsColorizeEffect>

char _init() {
    qRegisterMetaType<InventoryItem>();
    qRegisterMetaTypeStreamOperators<InventoryItem>("InventoryItem");
    QMetaType::registerComparators<InventoryItem>();
    return 0;
};
const auto _ = _init();

InventoryItem::InventoryItem(const QString &id) {
/*    qDebug() << "Calling normal constructor" << this; */
    setEmpty(id.isEmpty());
    if (!isEmpty()) {
        setNamespacedID(id);
    }
}

InventoryItem::InventoryItem() {
    /*qDebug() << "Calling void constructor" << this; */
    setEmpty(true);
}

InventoryItem::InventoryItem(const InventoryItem &other) {
/*    qDebug() << "Calling copy constructor" << this; */
    setEmpty(other.isEmpty());
    if (!isEmpty()) {
        setNamespacedID(other.getNamespacedID());
        setName((other.getName()));
    }
}

InventoryItem::~InventoryItem() {
    /*qDebug() << this << "is going to be deleted."; */
}

void InventoryItem::setupItem(QString id) {
    QString iconpath;
    QPixmap iconpix;

    Glhp::removePrefix(id, QStringLiteral("minecraft:"));

    if (id.endsWith(QStringLiteral("banner_pattern"))) {
        iconpath = ":/minecraft/texture/item/banner_pattern.png";
        iconpix  = QPixmap(iconpath);
    } else {
        iconpath = QStringLiteral(":minecraft/texture/item/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        iconpath = QStringLiteral(":minecraft/texture/inv_item/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        iconpath = QStringLiteral(":minecraft/texture/block/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        /*qWarning() << "unknown ID: " + id; */
        iconpix = QPixmap(16, 16);
        iconpix.fill(QStringLiteral("#FF00DC"));
        {
            QPainter painter(&iconpix);
            /*
               painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
               QBrush brush(QColor("#FF00DC"));
               painter.fillRect(iconpix.rect(), brush);
             */
            QBrush brush = QBrush(QColor(QStringLiteral("#000")));
            painter.fillRect(8, 0, 8, 8, brush);
            painter.fillRect(0, 8, 8, 8, brush);
            painter.end();
        }
    }
    iconpix = iconpix.scaled(32, 32, Qt::KeepAspectRatio);
    if (iconpix.size() != QSize(32, 32)) {
        QPixmap centeredPix(32, 32);
        centeredPix.fill(Qt::transparent);
        {
            QPainter painter(&centeredPix);
            painter.drawPixmap((32 - iconpix.width()) / 2,
                               (32 - iconpix.height()) / 2, iconpix);
            painter.end();
        }
        iconpix = centeredPix;
    }
    setPixmap(iconpix);

    const auto &MCRItemInfo  = MainWindow::getMCRInfo(QStringLiteral("item"));
    const auto &MCRBlockInfo = MainWindow::getMCRInfo(QStringLiteral("block"));
    if (MCRItemInfo.contains(id)) {
        setName(MCRItemInfo.value(id).toMap().value(QStringLiteral(
                                                        "name")).toString());
        m_flags = Item;
    } else if (MCRBlockInfo.contains(id)) {
        const auto blockMap = MCRBlockInfo.value(id).toMap();
        setIsItem(!blockMap.contains(QStringLiteral("unobtainable")));
        setName(blockMap.value(QStringLiteral("name")).toString());
        setIsBlock(true);
    }
}

bool InventoryItem::isEmpty() const {
    return m_isEmpty;
}

InventoryItem::Flags InventoryItem::getFlags() const {
    return m_flags;
}

void InventoryItem::setFlags(const Flags &flags) {
    m_flags = flags;
}

void InventoryItem::setEmpty(const bool &value) {
    m_isEmpty = value;
    if (isEmpty()) {
        m_namespacedId.clear();
        setName(QString());
        setIsBlock(false);
        m_flags &= ~Flag::Tag;
        m_pixmap = QPixmap();
    }
}

InventoryItem &InventoryItem::operator=(const InventoryItem &other) {
/*    qDebug() << "Calling operator ="; */

    if (&other == this)
        return *this;

    setEmpty(other.isEmpty());
    if (!isEmpty()) {
        setNamespacedID(other.getNamespacedID());
        setName((other.getName()));
    }

    return *this;
}

bool InventoryItem::operator==(const InventoryItem &other) {
    auto r = (getNamespacedID() == other.getNamespacedID());

    r = r && (getName() == other.getName());
    return r;
}

bool InventoryItem::operator==(const InventoryItem &other) const {
    auto r = (getNamespacedID() == other.getNamespacedID());

    r = r && (getName() == other.getName());
    return r;
}

bool InventoryItem::operator!=(const InventoryItem &other) {
    return !(*this == other);
}

bool InventoryItem::operator!=(const InventoryItem &other) const {
    return !(*this == other);
}

bool InventoryItem::operator<(const InventoryItem &other) const {
    if (getNamespacedID() == other.getNamespacedID())
        return getName() < other.getName();
    else
        return getNamespacedID() < other.getNamespacedID();
}

QString InventoryItem::getName() const {
    return this->m_name;
}

void InventoryItem::setName(const QString &name) {
    this->m_name = name;
}

QString InventoryItem::getNamespacedID() const {
/*    qDebug() << "getNamespacedID" << namespacedID; */
    return m_namespacedId;
}

void InventoryItem::setNamespacedID(const QString &id) {
/*    qDebug() << "setNamespacedID" << id; */
    if (id.isEmpty()) {
        qWarning() << "Can't set namespacedID to en empty InventoryItem";
        return;
    }

    setEmpty(id.isEmpty());

    if (id.startsWith("#")) {
        QPixmap iconpix(32, 32);
        iconpix.fill(Qt::transparent);
        {
            QPainter painter(&iconpix);
            QFont    font = painter.font();
            font.setPixelSize(32);
            painter.setFont(font);
            painter.drawText(QRect(0, 0, 32, 32),
                             Qt::AlignCenter,
                             QStringLiteral("#"));
            painter.end();
        }
        setPixmap(iconpix);
        auto idNoTag = id.mid(1);
        if (!idNoTag.contains(QStringLiteral(":")))
            this->m_namespacedId = QStringLiteral("#minecraft:") + idNoTag;
        else
            this->m_namespacedId = QStringLiteral("#") + idNoTag;
        setName(QCoreApplication::translate("InventoryItem",
                                            "Item tag: ") + id.midRef(1));
        m_flags |= Tag;
    } else {
        m_flags &= ~Tag;
        setupItem(id);
        if (!id.contains(':'))
            this->m_namespacedId = QStringLiteral("minecraft:") + id;
        else
            this->m_namespacedId = id;
    }
}

bool InventoryItem::isBlock() const {
    return m_flags.testFlag(Block);
}

void InventoryItem::setIsBlock(const bool &value) {
    m_flags.setFlag(Block, value);
}

bool InventoryItem::isItem() const {
    return m_flags.testFlag(Item);
}

void InventoryItem::setIsItem(const bool &value) {
    m_flags.setFlag(Item, value);
}

bool InventoryItem::isTag() const {
    return m_flags.testFlag(Tag);
}

QPixmap InventoryItem::getPixmap() const {
    return m_pixmap;
}

void InventoryItem::setPixmap(const QPixmap &value) {
    m_pixmap = value;
}

QString InventoryItem::toolTip() const {
    if (isEmpty()) {
        return QCoreApplication::translate("InventoryItem", "Empty item");
    } else if (!m_name.isEmpty()) {
        if (isTag())
            return m_name;
        else
            return m_name + "<br>" + QString("<br><code>%1</code>").arg(
                m_namespacedId);
    } else {
        return QCoreApplication::translate("InventoryItem",
                                           "Unknown item: ") + m_namespacedId;
    }
}

QDataStream &operator<<(QDataStream &out, const InventoryItem &obj) {
    out << obj.m_flags << obj.getNamespacedID() << obj.getName();
    return out;
}
QDataStream &operator>>(QDataStream &in, InventoryItem &obj) {
    InventoryItem::Flags flags;
    QString              namespacedID;
    QString              name;

    in >> flags >> namespacedID >> name;
    obj.m_flags = flags;
    obj.setNamespacedID(namespacedID);
    obj.setName(name);
    return in;
}

QDebug operator<<(QDebug debug, const InventoryItem &item) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "InventoryItem(";
    if (!item.isEmpty()) {
        debug.nospace() << item.getNamespacedID();
        if (!item.getName().isEmpty())
            debug.nospace() << ", " << item.getName();
    }
    debug.nospace() << ")";

    return debug;
}
