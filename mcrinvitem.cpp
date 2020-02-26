#include "mcrinvitem.h"

#include "mcrinvslot.h"
#include "mainwindow.h"

#include <QDebug>
#include <QPainter>
#include <QGraphicsColorizeEffect>

MCRInvItem::MCRInvItem(QWidget *parent, QString id) : QLabel(parent)
{   
    setMinimumSize(32, 32);

    QString iconpath;
    QPixmap iconpix;

    if(id.startsWith(QStringLiteral("minecraft:")))
        id.remove(0, 10);

    if(id == "debug_stick") {
        iconpath = ":minecraft/texture/item/stick.png";
        iconpix = QPixmap(iconpath);
    } else if(id == "enchanted_golden_apple") {
        iconpath = ":minecraft/texture/item/golden_apple.png";
        iconpix = QPixmap(iconpath);
    } else if(id == "potion" || id == "lingering_potion"
                             || id == "splash_potion") {
        QPixmap overlay = QPixmap(":minecraft/texture/item/potion_overlay.png");
        {
            QPainter painter(&overlay);
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.setOpacity(0.5);
            painter.setBackgroundMode(Qt::OpaqueMode);
            painter.fillRect(overlay.rect(), QBrush(QColor("#CA00CA")));
            painter.end();
        }
        iconpix = QPixmap(":minecraft/texture/item/"+id+".png");
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if(id == "leather_boots" || id == "leather_chestplate"
                                    || id == "leather_helmet"
                                    || id == "leather_leggings"
                                    || id == "leather_horse_armor") {
                iconpix = QPixmap(":minecraft/texture/item/"+id+".png");
        {
            QPainter painter(&iconpix);
//            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
//            painter.setOpacity(0.83);
//            painter.setBackgroundMode(Qt::OpaqueMode);
//            painter.fillRect(iconpix.rect(), QBrush(QColor("#A06540")));
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            QPixmap overlay = QPixmap(":minecraft/texture/item/"+id+"_overlay.png");
            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if(id == "firework_star") {
        QPixmap overlay = QPixmap(":minecraft/texture/item/firework_star_overlay.png");
        iconpix = QPixmap(":minecraft/texture/item/firework_star.png");
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else if(id == "tipped_arrow") {
        QPixmap overlay = QPixmap(":minecraft/texture/item/tipped_arrow_head.png");
        {
            QPainter painter(&overlay);
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.setOpacity(0.5);
            painter.setBackgroundMode(Qt::OpaqueMode);
            painter.fillRect(overlay.rect(), QBrush(QColor("#CA00CA")));
            painter.end();
        }
        iconpix = QPixmap(":minecraft/texture/item/tipped_arrow.png");
        {
            QPainter painter(&iconpix);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            painter.drawPixmap(overlay.rect(), overlay);
            painter.end();
        }
    } else {
        iconpath = ":minecraft/texture/item/"+id+".png";
        iconpix = QPixmap(iconpath);
    }

    if(!iconpix) {
        iconpath = ":minecraft/texture/block/"+id+".png";
        iconpix = QPixmap(iconpath);
    }

    if(!iconpix) {
        if(id.endsWith("_spawn_egg")) {
            iconpix = QPixmap(":minecraft/texture/item/spawn_egg.png");
            {
                QPainter painter(&iconpix);
                painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
                QPixmap overlay = QPixmap(":minecraft/texture/item/spawn_egg_overlay.png");
                painter.drawPixmap(overlay.rect(), overlay);
                painter.end();
            }
        }
    }

//    if(id == "vine") {
//        {
//            QPainter painter(&iconpix);
//            //painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
//            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
//            //QBrush brush = QBrush(QColor("#7748B518").darker(200));
//            QColor color = QColor("#7748B518");
//            //color.setHsv(color.hue(), color.saturation(), color.value()/2, color.alpha());
//            color.setHsv(color.hue(), qMin(color.saturation(), 1), color.value()*1, color.alpha());
//            qDebug() << color;
//            QBrush brush = QBrush(color);
//            painter.fillRect(iconpix.rect(), brush);
//            painter.end();
//        }
//    }

    if(!iconpix) {
        qDebug() << "unknown ID: " + id;
        iconpix = QPixmap(16, 16);
        iconpix.fill("#FF00DC");
        {
            QPainter painter(&iconpix);
            //painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
            //QBrush brush(QColor("#FF00DC"));
            //painter.fillRect(iconpix.rect(), brush);
            QBrush brush = QBrush(QColor("#000"));
            painter.fillRect(8, 0, 8, 8, brush);
            painter.fillRect(0, 8, 8, 8, brush);
            painter.end();
        }
    }
    iconpix = iconpix.scaled(32, 32, Qt::KeepAspectRatio);
    if(iconpix.size() != QSize(32, 32)) {
        QPixmap centeredPix(32, 32);
        centeredPix.fill(Qt::transparent);
        {
            QPainter painter(&centeredPix);
            painter.drawPixmap(
                (32 - iconpix.width()) / 2, 32 - iconpix.height(), iconpix);
            painter.end();
        }
        iconpix = centeredPix;
    }
    //qDebug() << iconpix;
    setPixmap(iconpix);
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_DeleteOnClose);

    auto MCRItemInfo = MainWindow::getMCRInfo("item");
    auto MCRBlockInfo = MainWindow::getMCRInfo("block");
    if(MCRItemInfo->contains(id)) {
        setName(MCRItemInfo->value(id).toMap().value("name").toString());
    } else if(MCRBlockInfo->contains(id)) {
        auto blockMap = MCRBlockInfo->value(id).toMap();
        if(!blockMap.contains("unobtainable"))
            setName(blockMap.value("name").toString());
    }

    if(!name.isEmpty()) {
        setToolTip(name);
    } else {
        setToolTip("Unknown item: "+id);
    }
    //invItem->show();

    this->namespacedID = id;

    show();

    setupItem();
}

void MCRInvItem::setupItem() {

}

QString MCRInvItem::getName() {
    return this->name;
}

void MCRInvItem::setName(const QString &name) {
    this->name = name;
}
