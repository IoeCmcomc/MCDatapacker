#include "advancementtab.h"

#include "layouttreenode.h"
#include "advancementitem.h"

#include <QGraphicsPixmapItem>
#include <QRectF>

AdvancementTab::AdvancementTab(QWidget *parent) : QGraphicsView(parent) {
    setScene(&m_scene);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setCacheMode(QGraphicsView::CacheBackground);
}

void AdvancementTab::setRootAdvancement(AdvancemDisplayInfo &&advancem,
                                        const QString &id) {
    m_rootAdvancement = advancem;
    m_rootAdvancemId  = id;

    if (isHidden()) {
        m_pendingLoad = true;
        return;
    }

    arrangeItems();
}

void AdvancementTab::showEvent(QShowEvent *event) {
    QGraphicsView::showEvent(event);
    if (m_pendingLoad) {
        m_pendingLoad = false;
        arrangeItems();
    }
}

LayoutTreeNode * AdvancementTab::addAdvancement(
    const AdvancemDisplayInfo &advancem, const QString &id) {
    auto *item = new AdvancementItem(advancem, id);

    m_scene.addItem(item);

    auto *layoutNode = new LayoutTreeNode(item);

    for (const auto &[childId, child]: advancem.children) {
        layoutNode->add(addAdvancement(child, childId));
    }

    return layoutNode;
}

void AdvancementTab::arrangeItems() {
    m_scene.clear();

    m_layoutTree =
        std::unique_ptr<LayoutTreeNode>(addAdvancement(m_rootAdvancement,
                                                       m_rootAdvancemId));

    m_layoutTree->update();
    drawConnections(m_layoutTree.get());
    m_scene.setSceneRect(m_scene.itemsBoundingRect());
}

void AdvancementTab::drawConnections(LayoutTreeNode *node) {
    for (auto const &child: node->children()) {
        drawConnections(child.get());
    }

    const auto  *item = node->item();
    const QRectF rect{ item->pos(), item->pixmap().size() };
    const auto  &center = rect.center();

    if (!node->children().empty()) {
        QPainterPath path{ center };

        if (node->children().size() == 1) {
            path.lineTo(center.x(), node->getFirst()->getCenterY());
        } else {
            const int spanConnY =
                rect.bottom() + LayoutTreeNode::ySpacing / 2;

            path.lineTo(center.x(), spanConnY);
            path.moveTo(node->getFirst()->getCenterX(), spanConnY);
            path.lineTo(node->getLast()->getCenterX(), spanConnY);

            for (auto const &child: node->children()) {
                const int childCenterX = child->getCenterX();
                path.moveTo(childCenterX, child->getCenterY());
                path.lineTo(childCenterX, spanConnY);
            }
        }

        QPainterPathStroker stroker;
        stroker.setWidth(4);
        const auto &&connPath = stroker.createStroke(path).simplified();
        QPen         pen{ Qt::black };
        pen.setWidth(2);
        m_scene.addPath(connPath, pen, Qt::white);
    }
}
