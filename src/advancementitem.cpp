#include "advancementitem.h"

#include "advancementtab.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QFileInfo>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QApplication>
#include <QClipboard>

AdvancementItem::AdvancementItem(const AdvancemDisplayInfo &advancem,
                                 const QString &id) : m_id{id} {
    using FrameType = AdvancemDisplayInfo::FrameType;

    QPixmap frame;
    switch (advancem.frameType) {
        case FrameType::Task: {
            static const QPixmap task{
                QLatin1String(":/minecraft/texture/advancement/regular_task.png") };
            frame = task;
            break;
        }
        case FrameType::Goal: {
            static const QPixmap goal{
                QLatin1String(":/minecraft/texture/advancement/regular_goal.png") };
            frame = goal;
            break;
        }
        case FrameType::Challenge: {
            static const QPixmap challenge{
                QLatin1String(
                    ":/minecraft/texture/advancement/regular_challenge.png") };
            frame = challenge;
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
    return AdvancementItem::Type;
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

void AdvancementItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu    menu;
    QAction *copyIdAction = menu.addAction(
        QCoreApplication::translate("AdvancementItem",
                                    "Copy resouce location"));

    scene()->connect(copyIdAction, &QAction::triggered,
                     copyIdAction, [ this ](){
        qApp->clipboard()->setText(m_id);
    });

    event->accept();
    menu.exec(event->screenPos());
}
