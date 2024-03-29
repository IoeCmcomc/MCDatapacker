#include "inventoryitem.h"

#include "game.h"
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
[[maybe_unused]] const auto _ = _init();

InventoryItem::InventoryItem(const QString &id) {
/*    qDebug() << "Calling normal constructor" << this; */
    if (!id.isEmpty())
        setNamespacedID(id);
}

InventoryItem::InventoryItem(InventoryItem &&other)
    : m_pixmap{std::move(other.m_pixmap)},
    m_name{std::move(other.m_name)},
    m_namespacedId{std::move(other.m_namespacedId)},
    m_flags{other.m_flags} {
}

InventoryItem &InventoryItem::operator=(InventoryItem &&other) {
    if (this != &other) {
        m_flags        = std::exchange(other.m_flags, Null);
        m_pixmap       = std::move(other.m_pixmap);
        m_name         = std::move(other.m_name);
        m_namespacedId = std::move(other.m_namespacedId);
    }
    return *this;
}

void InventoryItem::setupItem(QString id) {
    Glhp::removePrefix(id, "minecraft:"_QL1);

    const auto &&MCRItemInfo  = Game::getInfo(QStringLiteral("item"));
    const auto &&MCRBlockInfo = Game::getInfo(QStringLiteral("block"));

    if (MCRItemInfo.contains(id)) {
        setName(MCRItemInfo.value(id).toMap().value(QStringLiteral(
                                                        "name")).toString());
        m_flags = Item;
    } else if (MCRBlockInfo.contains(id)) {
        const auto &blockMap = MCRBlockInfo.value(id).toMap();
        setIsItem(!blockMap.contains(QStringLiteral("unobtainable")));
        setName(blockMap.value(QStringLiteral("name")).toString());
        setIsBlock(true);
    }
}

QPixmap InventoryItem::loadPixmap(QString id) const {
    if (id.isEmpty()) {
        return {};
    }

    if (isTag()) {
        QPixmap iconpix(32, 32);
        iconpix.fill(Qt::transparent);
        {
            QPainter painter(&iconpix);
            QFont    font = painter.font();
            font.setPixelSize(32);
            painter.setFont(font);
            painter.drawText(QRect(0, 0, 32, 32), Qt::AlignCenter,
                             QStringLiteral("#"));
            painter.end();
        }
        return iconpix;
    }


    QPixmap iconpix;
    QString iconpath;

    Glhp::removePrefix(id, "minecraft:"_QL1);
    const auto &&MCRItemInfo = Game::getInfo(QStringLiteral("item"));

    if (id.endsWith(QLatin1String("banner_pattern"))) {
        iconpath = ":/minecraft/texture/item/banner_pattern.png";
        iconpix  = QPixmap(iconpath);
    } else if (MCRItemInfo.contains(id)) {
        iconpath = QStringLiteral(":minecraft/texture/item/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    } else {
        iconpath = QStringLiteral(":minecraft/texture/inv_item/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        iconpath = QStringLiteral(":minecraft/texture/block/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) { // Draw missing texture
        const static QColor magenta = QColor(248, 0, 248);
        iconpix = QPixmap(16, 16);
        iconpix.fill(magenta);
        {
            QPainter            painter(&iconpix);
            const static QBrush brush(Qt::black);
            painter.fillRect(0, 0, 8, 8, brush);
            painter.fillRect(8, 8, 8, 8, brush);
            painter.end();
        }
    }
    static const int   pixmapLength = 32;
    static const QSize pixmapSize(pixmapLength, pixmapLength);
    iconpix = iconpix.scaled(pixmapLength, pixmapLength, Qt::KeepAspectRatio);
    if (iconpix.size() != pixmapSize) {
        QPixmap centeredPix(pixmapLength, pixmapLength);
        centeredPix.fill(Qt::transparent);
        {
            QPainter painter(&centeredPix);
            painter.drawPixmap((pixmapLength - iconpix.width()) / 2,
                               (pixmapLength - iconpix.height()) / 2, iconpix);
            painter.end();
        }
        iconpix = centeredPix;
    }

    return iconpix;
}

bool InventoryItem::isNull() const {
    return m_flags == Null;
}

InventoryItem::Flags InventoryItem::getFlags() const {
    return m_flags;
}

void InventoryItem::setFlags(const Flags &flags) {
    m_flags = flags;
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
        qWarning() << "Empty namespaced ID";
        return;
    }

    if (id.startsWith('#')) {
        const auto idNoTag = id.midRef(1);
        if (!idNoTag.contains(':'))
            this->m_namespacedId = QStringLiteral("#minecraft:") + idNoTag;
        else
            this->m_namespacedId = QStringLiteral("#") + idNoTag;
        setName(QCoreApplication::translate("InventoryItem",
                                            "Item tag: ") + idNoTag);
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
    if (!m_pixmap) {
        m_pixmap = loadPixmap(m_namespacedId);
    }
    return m_pixmap;
}

void InventoryItem::setPixmap(const QPixmap &newPixmap) {
    m_pixmap = newPixmap;
}


QString InventoryItem::toolTip() const {
    if (isNull()) {
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
    if (!item.isNull()) {
        debug.nospace() << item.getNamespacedID();
        if (!item.getName().isEmpty())
            debug.nospace() << ", " << item.getName();
    }
    debug.nospace() << ')';

    return debug;
}
