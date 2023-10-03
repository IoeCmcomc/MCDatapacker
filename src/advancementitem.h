#ifndef ADVANCEMENTITEM_H
#define ADVANCEMENTITEM_H

#include "advancementtab.h"

#include <QGraphicsPixmapItem>

class AdvancementItem : public QGraphicsPixmapItem {
public:
    AdvancementItem(const AdvancemDisplayInfo &advancem, const QString &id);

    int type() const final;

    static constexpr int Type = QGraphicsItem::UserType + 1;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) final;

private:
    const QString m_id;
};

#endif // ADVANCEMENTITEM_H
