#ifndef ADVANCEMENTTAB_H
#define ADVANCEMENTTAB_H

#include "advancementtabdock.h"

#include <QGraphicsView>

class LayoutTreeNode;
class AdvancementItem;

class AdvancementTab : public QGraphicsView {
    Q_OBJECT
public:
    explicit AdvancementTab(QWidget *parent = nullptr);

    void setRootAdvancement(AdvancemDisplayInfo &&advancem, const QString &id);

signals:
    void openFileRequested(const QString &filePath);

private:
    AdvancemDisplayInfo m_rootAdvancement;
    QGraphicsScene m_scene;
    QVector<AdvancementItem *> m_items;
    std::unique_ptr<LayoutTreeNode> m_layoutTree = nullptr;
    QString m_rootAdvancemId;

    LayoutTreeNode * addAdvancement(
        const AdvancemDisplayInfo &advancem, const QString &id);
    void arrangeItems();
    void drawConnections(LayoutTreeNode *node);
};

#endif // ADVANCEMENTTAB_H
