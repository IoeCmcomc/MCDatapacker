#ifndef ADVANCEMENTTAB_H
#define ADVANCEMENTTAB_H

#include "layouttreenode.h"

#include <QGraphicsView>
#include <QJsonValue>

class LayoutTreeNode;
class AdvancementItem;

struct AdvancemDisplayInfo {
    enum class FrameType: char {
        Task,
        Goal,
        Challenge,
    };

    QPixmap                                displayIcon;
    std::map<QString, AdvancemDisplayInfo> children;
    QJsonValue                             title;
    QJsonValue                             description;
    QString                                backgroundPath;
    QString                                parent;
    FrameType                              frameType = FrameType::Task;
    bool                                   missing   = false;
    bool                                   hidden    = false;
};

class AdvancementTab : public QGraphicsView {
    Q_OBJECT
public:
    explicit AdvancementTab(QWidget *parent = nullptr);

    void setRootAdvancement(AdvancemDisplayInfo &&advancem, const QString &id);

signals:
    void openFileRequested(const QString &filePath);

protected:
    void showEvent(QShowEvent *event) final;

private:
    AdvancemDisplayInfo m_rootAdvancement;
    QGraphicsScene m_scene;
    std::unique_ptr<LayoutTreeNode> m_layoutTree;
    QString m_rootAdvancemId;
    bool m_pendingLoad = false;

    LayoutTreeNode * addAdvancement(
        const AdvancemDisplayInfo &advancem, const QString &id);
    void arrangeItems();
    void drawConnections(LayoutTreeNode *node);
};

#endif // ADVANCEMENTTAB_H
