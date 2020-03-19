#include "mcrinvitem.h"

#include "mcrinvslot.h"
#include "mainwindow.h"

#include <QPainter>
#include <QApplication>
#include <QGraphicsColorizeEffect>

MCRInvItem::MCRInvItem(QString id) {
/*    qDebug() << "Calling normal constructor" << this; */
    qRegisterMetaTypeStreamOperators<MCRInvItem>("MCRInvItem");

    setEmpty(id.isEmpty());
    if (!isEmpty()) {
        setNamespacedID(id);
    }
}

MCRInvItem::MCRInvItem() {
    /*qDebug() << "Calling void constructor" << this; */
    qRegisterMetaTypeStreamOperators<MCRInvItem>("MCRInvItem");
    setEmpty(true);
}

MCRInvItem::MCRInvItem(const MCRInvItem &other) {
/*    qDebug() << "Calling copy constructor" << this; */
    qRegisterMetaTypeStreamOperators<MCRInvItem>("MCRInvItem");

    setEmpty(other.isEmpty());
    if (!isEmpty()) {
        setNamespacedID(other.getNamespacedID());
        setName((other.getName()));
    }
}

MCRInvItem::~MCRInvItem() {
    /*qDebug() << this << "is going to be deleted."; */
}

void MCRInvItem::setupItem(QString id) {
    QString iconpath;
    QPixmap iconpix;

    if (id.startsWith(QStringLiteral("minecraft:")))
        id.remove(0, 10);

    if (id == QStringLiteral("debug_stick")) {
        iconpath = QStringLiteral(":minecraft/texture/item/stick.png");
        iconpix  = QPixmap(iconpath);
    } else if (id == QStringLiteral("enchanted_golden_apple")) {
        iconpath = QStringLiteral(":minecraft/texture/item/golden_apple.png");
        iconpix  = QPixmap(iconpath);
    } else if (id == QStringLiteral("potion") ||
               id == QStringLiteral("lingering_potion")
               || id == QStringLiteral("splash_potion")) {
        QPixmap overlay = QPixmap(":minecraft/texture/item/potion_overlay.png");
        {
            QPainter painter(&overlay);
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.setOpacity(0.5);
            painter.setBackgroundMode(Qt::OpaqueMode);
            painter.fillRect(overlay.rect(),
                             QBrush(QColor(QStringLiteral("#CA00CA"))));
            painter.end();
        }
        iconpix = QPixmap(QStringLiteral(
                              ":minecraft/texture/item/") + id +
                          QStringLiteral(".png"));
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if (id == QStringLiteral("leather_boots")
               || id == QStringLiteral("leather_chestplate")
               || id == QStringLiteral("leather_helmet")
               || id == QStringLiteral("leather_leggings")
               || id == QStringLiteral("leather_horse_armor")) {
        iconpix = QPixmap(QStringLiteral(
                              ":minecraft/texture/item/") + id +
                          QStringLiteral(".png"));
        {
            QPainter painter(&iconpix);
/*
              painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
              painter.setOpacity(0.83);
              painter.setBackgroundMode(Qt::OpaqueMode);
              painter.fillRect(iconpix.rect(), QBrush(QColor("#A06540")));
 */
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            QPixmap overlay = QPixmap(
                QStringLiteral(":minecraft/texture/item/") + id + QStringLiteral(
                    "_overlay.png"));
            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if (id == QStringLiteral("firework_star")) {
        QPixmap overlay = QPixmap(
            QStringLiteral(":minecraft/texture/item/firework_star_overlay.png"));
        iconpix =
            QPixmap(QStringLiteral(":minecraft/texture/item/firework_star.png"));
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if (id == QStringLiteral("tipped_arrow")) {
        QPixmap overlay = QPixmap(
            QStringLiteral(":minecraft/texture/item/tipped_arrow_head.png"));
        {
            QPainter painter(&overlay);
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.setOpacity(0.5);
            painter.setBackgroundMode(Qt::OpaqueMode);
            painter.fillRect(overlay.rect(),
                             QBrush(QColor(QStringLiteral("#CA00CA"))));
            painter.end();
        }
        iconpix =
            QPixmap(QStringLiteral(":minecraft/texture/item/tipped_arrow.png"));
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else {
        iconpath = QStringLiteral(":minecraft/texture/item/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        iconpath = QStringLiteral(":minecraft/texture/block/") + id +
                   QStringLiteral(".png");
        iconpix = QPixmap(iconpath);
    }

    if (!iconpix) {
        if (id.endsWith(QStringLiteral("_spawn_egg"))) {
            iconpix =
                QPixmap(QStringLiteral(":minecraft/texture/item/spawn_egg.png"));
            {
                QPainter painter(&iconpix);
                painter.setCompositionMode(
                    QPainter::CompositionMode_DestinationOver);
                QPixmap overlay = QPixmap(
                    QStringLiteral(
                        ":minecraft/texture/item/spawn_egg_overlay.png"));
                painter.drawPixmap(overlay.rect(), overlay);
                painter.end();
            }
        }
    }

/*
      if(id == "vine") {
          {
              QPainter painter(&iconpix);
              //painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
              painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
              //QBrush brush = QBrush(QColor("#7748B518").darker(200));
              QColor color = QColor("#7748B518");
              //color.setHsv(color.hue(), color.saturation(), color.value()/2, color.alpha());
              color.setHsv(color.hue(), qMin(color.saturation(), 1), color.value()*1, color.alpha());
              qDebug() << color;
              QBrush brush = QBrush(color);
              painter.fillRect(iconpix.rect(), brush);
              painter.end();
          }
      }
 */

    if (!iconpix) {
        qDebug() << "unknown ID: " + id;
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

    auto MCRItemInfo  = MainWindow::getMCRInfo(QStringLiteral("item"));
    auto MCRBlockInfo = MainWindow::getMCRInfo(QStringLiteral("block"));
    if (MCRItemInfo.contains(id)) {
        setName(MCRItemInfo.value(id).toMap().value(QStringLiteral(
                                                        "name")).toString());
        setHasBlockForm(false);
    } else if (MCRBlockInfo.contains(id)) {
        auto blockMap = MCRBlockInfo.value(id).toMap();
        if (!blockMap.contains(QStringLiteral("unobtainable")))
            setName(blockMap.value(QStringLiteral("name")).toString());
        setHasBlockForm(true);
    }
}

bool MCRInvItem::isEmpty() const {
    return m_isEmpty;
}

void MCRInvItem::setEmpty(const bool &value) {
    m_isEmpty = value;
    if (isEmpty()) {
        namespacedID.clear();
        setName(QString());
        setHasBlockForm(false);
        isTag  = false;
        pixmap = QPixmap();
    }
}

MCRInvItem &MCRInvItem::operator=(const MCRInvItem &other) {
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

bool MCRInvItem::operator==(const MCRInvItem &other) {
    auto r = (getNamespacedID() == other.getNamespacedID());

    r = r && (getName() == other.getName());
    return r;
}

bool MCRInvItem::operator==(const MCRInvItem &other) const {
    auto r = (getNamespacedID() == other.getNamespacedID());

    r = r && (getName() == other.getName());
    return r;
}

bool MCRInvItem::operator!=(const MCRInvItem &other) {
    return !(*this == other);
}

bool MCRInvItem::operator!=(const MCRInvItem &other) const {
    return !(*this == other);
}

bool MCRInvItem::operator<(const MCRInvItem &other) const {
    if (getNamespacedID() == other.getNamespacedID())
        return getName() < other.getName();
    else
        return getNamespacedID() < other.getNamespacedID();
}

QString MCRInvItem::getName() const {
    return this->name;
}

void MCRInvItem::setName(const QString &name) {
    this->name = name;
}

QString MCRInvItem::getNamespacedID() const {
/*    qDebug() << "getNamespacedID" << namespacedID; */
    return namespacedID;
}

void MCRInvItem::setNamespacedID(const QString &id) {
/*    qDebug() << "setNamespacedID" << id; */
    if (id.isEmpty()) {
        qWarning() << "Can't set namespacedID to en empty MCRInvItem";
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
            this->namespacedID = QStringLiteral("#minecraft:") + idNoTag;
        else
            this->namespacedID = QStringLiteral("#") + idNoTag;
        setName(QCoreApplication::translate("MCRInvItem",
                                            "Item tag: ") + id.midRef(1));
        isTag = true;
    } else {
        isTag = false;
        setupItem(id);
        if (!id.contains(":"))
            this->namespacedID = QStringLiteral("minecraft:") + id;
        else
            this->namespacedID = id;
    }
}

bool MCRInvItem::getHasBlockForm() const {
    return hasBlockForm;
}

void MCRInvItem::setHasBlockForm(const bool &value) {
    hasBlockForm = value;
}

bool MCRInvItem::getIsTag() const {
    return isTag;
}

QPixmap MCRInvItem::getPixmap() const {
    return pixmap;
}

void MCRInvItem::setPixmap(const QPixmap &value) {
    pixmap = value;
}

QString MCRInvItem::toolTip() {
    if (isEmpty()) {
        return QCoreApplication::translate("MCRInvItem", "Empty item");
    } else if (!name.isEmpty()) {
        return name;
    } else {
        return QCoreApplication::translate("MCRInvItem",
                                           "Unknown item: ") + namespacedID;
    }
}

QDataStream &operator<<(QDataStream &out, const MCRInvItem &obj) {
    out << obj.getNamespacedID() << obj.getName();
    return out;
}
QDataStream &operator>>(QDataStream &in, MCRInvItem &obj) {
    QString namespacedID;
    QString name;

    in >> namespacedID >> name;
    obj.setNamespacedID(namespacedID);
    obj.setName(name);
    return in;
}

QDebug operator<<(QDebug debug, const MCRInvItem &item) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "MCRInvItem(";
    if (!item.isEmpty()) {
        debug.nospace() << item.getNamespacedID();
        if (!item.getName().isEmpty())
            debug.nospace() << ", " << item.getName();
    }
    debug.nospace() << ")";

    return debug;
}
