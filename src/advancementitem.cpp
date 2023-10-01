#include "advancementitem.h"

#include "advancementtab.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QFileInfo>

AdvancementItem::AdvancementItem(const AdvancemDisplayInfo &advancem,
                                 const QString &id) : m_id{id} {
    using FrameType = AdvancemDisplayInfo::FrameType;

    QPixmap frame;
    switch (advancem.frameType) {
        case FrameType::Task: {
            frame.load(":/minecraft/texture/advancement/regular_task.png");
            break;
        }
        case FrameType::Goal: {
            frame.load(":/minecraft/texture/advancement/regular_goal.png");
            break;
        }
        case FrameType::Challenge: {
            frame.load(
                ":/minecraft/texture/advancement/regular_challenge.png");
            break;
        }
    }
    {
        QPainter painter{ &frame };
        painter.drawPixmap(10, 10, advancem.displayIcon);
    }

    setPixmap(frame);
    setToolTip(id);
    setZValue(1);
    setOpacity((advancem.hidden && !advancem.parent.isEmpty()) ? 0.7 : 1);
    setCursor(Qt::ArrowCursor);
}

int AdvancementItem::type() const {
    return QGraphicsItem::UserType + 1;
}

void AdvancementItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);

    QFileInfo finfo{
        QStringLiteral("data/") + m_id.section(":", 0, 0)
        + QStringLiteral("/advancements/") +
        m_id.section(":", 1, 1) + QStringLiteral(".json") };

    if (finfo.exists() && finfo.isFile()) {
        emit qobject_cast<AdvancementTab *>(scene()->views().front())
        ->openFileRequested(finfo.absoluteFilePath());
    }
}
