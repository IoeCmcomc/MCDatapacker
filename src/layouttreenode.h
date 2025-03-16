#ifndef LAYOUTTREENODE_H
#define LAYOUTTREENODE_H

#include <QPoint>

#include <vector>
#include <memory>

class QGraphicsPixmapItem;

class LayoutTreeNode {
/*
 * This class implement Buchhiem tree layout algorithm to determine
 * nodes' position, which may prodives different results from the tree layout
 * used in Minecraft's advancement tabs.
 * The actual code is based on this Python version (WTFPL license):
 * https://github.com/llimllib/pymag-trees/blob/master/buchheim.py
 */
public:
    LayoutTreeNode(QGraphicsPixmapItem *newItem);

    LayoutTreeNode * get(int i);
    void add(LayoutTreeNode *ptr);

    bool isRoot();
    bool isLeaf();

    LayoutTreeNode * getFirst();
    LayoutTreeNode * getLast();

    bool isFirst();
    bool isLast();

    LayoutTreeNode * getNext();
    LayoutTreeNode * getPrev();

    LayoutTreeNode * getLeft();
    LayoutTreeNode * getRight();

    LayoutTreeNode * firstSibling();

    void update();

    QGraphicsPixmapItem * item() const;
    int getCenterX() const;
    int getCenterY() const;

    std::vector<std::unique_ptr<LayoutTreeNode> > &children();

    static constexpr int xSpacing = 8;
    static constexpr int ySpacing = 8;

private:
    std::vector<std::unique_ptr<LayoutTreeNode> > m_children;
    LayoutTreeNode *m_thread    = nullptr;
    LayoutTreeNode *m_parent    = nullptr;
    LayoutTreeNode *m_ancestor  = this;
    QGraphicsPixmapItem *m_item = nullptr;
    float m_col                 = -1;
    float m_mod                 = 0;
    float m_shift               = 0;
    float m_change              = 0;
    int m_index                 = 0;
    int m_depth                 = 0;

    void firstWalk(const float distance = 1.0);
    LayoutTreeNode * apportion(LayoutTreeNode *defaultAncestor,
                               float distance);
    static void moveSubtree(LayoutTreeNode *wl, LayoutTreeNode *wr,
                            float shift);
    void execShifts();
    LayoutTreeNode * getAncestor(LayoutTreeNode *vil,
                                 LayoutTreeNode *defaultAncestor);
    float secondWalk(float m   = 0, int newDepth = 0,
                     float min = std::numeric_limits<float>::quiet_NaN());
    void thirdWalk(float n);
    void updateItem();
};

#endif // LAYOUTTREENODE_H
