#include "layouttreenode.h"

#include <QGraphicsPixmapItem>

#include <cmath>

LayoutTreeNode::LayoutTreeNode(QGraphicsPixmapItem *newItem) : m_item{newItem} {
}

LayoutTreeNode * LayoutTreeNode::get(int i) {
    return m_children[i].get();
}

void LayoutTreeNode::add(LayoutTreeNode *ptr) {
    ptr->m_parent = this;
    ptr->m_depth  = m_depth + 1;
    ptr->m_index  = m_children.size();
    m_children.emplace_back(ptr);
}

bool LayoutTreeNode::isRoot() {
    return m_parent == nullptr;
}

bool LayoutTreeNode::isLeaf() {
    return m_children.empty();
}

LayoutTreeNode * LayoutTreeNode::getFirst() {
    return (!this->isLeaf()) ? this->get(0) : nullptr;
}

LayoutTreeNode * LayoutTreeNode::getLast() {
    return (!this->isLeaf()) ? this->get(this->m_children.size() -
                                         1) : nullptr;
}

bool LayoutTreeNode::isFirst() {
    return (!isRoot()) ? (this->m_parent->getFirst() ==
                          this) : true;
}

bool LayoutTreeNode::isLast() {
    return (!isRoot()) ? (this->m_parent->getLast() ==
                          this) : true;
}

LayoutTreeNode * LayoutTreeNode::getNext() {
    if ((!isRoot()) && (!this->isLast())) {
        return this->m_parent->get(m_index + 1);
    } else {
        return nullptr;
    }
}

LayoutTreeNode * LayoutTreeNode::getPrev() {
    if ((!isRoot()) && (!this->isFirst())) {
        return this->m_parent->get(m_index - 1);
    } else {
        return nullptr;
    }
}

LayoutTreeNode * LayoutTreeNode::getLeft() {
    return (this->isLeaf()) ? this->m_thread : this->getFirst();
}

LayoutTreeNode * LayoutTreeNode::getRight() {
    return (this->isLeaf()) ? this->m_thread : this->getLast();
}

LayoutTreeNode * LayoutTreeNode::firstSibling() {
    if ((!isRoot()) && (!this->isFirst())) {
        return this->m_parent->getFirst();
    } else {
        return nullptr;
    }
}

void LayoutTreeNode::update() {
    firstWalk();
    float min = secondWalk();

    if (min < 0)
        thirdWalk(-min);

    updateItem();
}

QGraphicsPixmapItem * LayoutTreeNode::item() const {
    return m_item;
}

int LayoutTreeNode::getCenterX() const {
    return item()->x() + item()->pixmap().width() / 2;
}

int LayoutTreeNode::getCenterY() const {
    return item()->y() + item()->pixmap().height() / 2;
}

std::vector<std::unique_ptr<LayoutTreeNode> > &LayoutTreeNode::children() {
    return m_children;
}

void LayoutTreeNode::firstWalk(const float distance) {
    m_mod    = 0;
    m_shift  = 0;
    m_change = 0;
    //ancestor = null;
    m_thread = nullptr;
    if (isLeaf()) {
        if (!isFirst())
            m_col = getPrev()->m_col + distance;
        else
            m_col = 0;
    } else {
        auto *defaultAncestor = getFirst();
        for (auto const &w: m_children) {
            w->firstWalk();
            defaultAncestor = w->apportion(defaultAncestor, distance);
        }
        execShifts();

        float midpoint = (getFirst()->m_col + getLast()->m_col) / 2;

        const auto *w = getPrev();
        if (w != nullptr) {
            m_col = w->m_col + distance;
            m_mod = m_col - midpoint;
        } else {
            m_col = midpoint;
        }
    }
}

LayoutTreeNode * LayoutTreeNode::apportion(LayoutTreeNode *defaultAncestor,
                                           float distance) {
    LayoutTreeNode *w = getPrev();

    if (w != nullptr) {
        // in buchheim notation:
        // i == inner; o == outer;
        // r == right; l == left;
        auto *vir = this;
        auto *vor = this;
        auto *vil = w;
        auto *vol = firstSibling();
        float sir = m_mod;
        float sor = m_mod;
        float sil = vil->m_mod;
        float sol = vol->m_mod;
        while ((vil->getRight() != nullptr) &&
               (vir->getLeft() != nullptr)) {
            vil             = vil->getRight();
            vir             = vir->getLeft();
            vol             = vol->getLeft();
            vor             = vor->getRight();
            vor->m_ancestor = this;
            float shift = (vil->m_col + sil) - (vir->m_col + sir) + distance;
            if (shift > 0) {
                moveSubtree(getAncestor(vil, defaultAncestor), this, shift);
                sir += shift;
                sor += shift;
            }
            sil += vil->m_mod;
            sir += vir->m_mod;
            sol += vol->m_mod;
            sor += vor->m_mod;
        }
        if ((vil->getRight() != nullptr) && (vor->getRight() == nullptr)) {
            vor->m_thread = vil->getRight();
            vor->m_mod    = sil - sor;
        } else {
            if ((vir->getLeft() != nullptr) &&
                (vol->getLeft() == nullptr)) {
                vol->m_thread = vir->getLeft();
                vol->m_mod    = sir - sol;
            }
            defaultAncestor = this;
        }
    }
    return defaultAncestor;
}

void LayoutTreeNode::moveSubtree(LayoutTreeNode *wl,
                                 LayoutTreeNode *wr,
                                 float shift) {
    int subtrees = wr->m_index - wl->m_index;

    wr->m_change -= shift / subtrees;
    wr->m_shift  += shift;
    wl->m_change += shift / subtrees;
    wr->m_col    += shift;
    wr->m_mod    += shift;
}

void LayoutTreeNode::execShifts() {
    float shift  = 0;
    float change = 0;

    for (int i = m_children.size() - 1; i >= 0; --i) {
        auto *w = get(i);
        w->m_col += shift;
        w->m_mod += shift;
        change   += w->m_change;
        shift    += w->m_shift + change;
    }
}

LayoutTreeNode * LayoutTreeNode::getAncestor(LayoutTreeNode *vil,
                                             LayoutTreeNode *defaultAncestor) {
    const auto &iter = std::find_if(m_parent->m_children.cbegin(),
                                    m_parent->m_children.cend(),
                                    [vil](const auto &value){
        return vil->m_ancestor == value.get();
    });

    //        if (parent->children.contains(vil->ancestor)) {
    if (iter != m_parent->m_children.cend()) {
        return vil->m_ancestor;
    } else {
        return defaultAncestor;
    }
}

float LayoutTreeNode::secondWalk(float m, int newDepth, float min) {
    m_col  += m;
    m_depth = newDepth;

    if (std::isnan(min) || (m_col < min)) {
        min = m_col;
    }

    for (auto const &w: m_children) {
        w->secondWalk(m + m_mod, m_depth + 1, min);
    }

    return min;
}

void LayoutTreeNode::thirdWalk(float n) {
    m_col += n;
    for (auto const &c: m_children)
        c->thirdWalk(n);
}

void LayoutTreeNode::updateItem() {
    m_item->setPos(m_col * (m_item->pixmap().width() + xSpacing),
                   m_depth * (m_item->pixmap().height() + ySpacing));
    for (auto const &child: m_children)
        child->updateItem();
}
