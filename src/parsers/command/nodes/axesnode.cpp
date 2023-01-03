#include "axesnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    TwoAxesNode::TwoAxesNode(ParserType parserType, int length)
        : ArgumentNode(parserType, length) {
    }

    void TwoAxesNode::_accept(NodeVisitor *visitor, VisitOrder order) {
        if (m_first)
            m_first->accept(visitor, order);
        if (m_second)
            m_second->accept(visitor, order);
    }

    AnglePtr TwoAxesNode::firstAxis() const {
        return m_first;
    }

    void TwoAxesNode::setFirstAxis(AnglePtr axis) {
        m_first = std::move(axis);
    }

    AnglePtr TwoAxesNode::secondAxis() const {
        return m_second;
    }

    void TwoAxesNode::setSecondAxis(AnglePtr axis) {
        m_second = std::move(axis);
    }

    XyzNode::XyzNode(ParserType parserType, int length)
        : ArgumentNode(parserType, length) {
    }

    void XyzNode::_accept(NodeVisitor *visitor, VisitOrder order) {
        if (m_x)
            m_x->accept(visitor, order);
        if (m_y)
            m_y->accept(visitor, order);
        if (m_z)
            m_z->accept(visitor, order);
    }

    AnglePtr XyzNode::x() const {
        return m_x;
    }

    void XyzNode::setX(AnglePtr x) {
        m_x = std::move(x);
    }

    AnglePtr XyzNode::y() {
        return m_y;
    }
    void XyzNode::setY(AnglePtr y) {
        m_y = std::move(y);
    }

    AnglePtr XyzNode::z() const {
        return m_z;
    }

    void XyzNode::setZ(AnglePtr z) {
        m_z = std::move(z);
    }

#define DECLARE_NODE_CLASS(Class)                                     \
        void Class::accept(NodeVisitor * visitor, VisitOrder order) { \
            if (order == VisitOrder::LetTheVisitorDecide) {           \
                visitor->visit(this);                                 \
                return;                                               \
            }                                                         \
            if (order == VisitOrder::Preorder)                        \
            visitor->visit(this);                                     \
            _accept(visitor, order);                                  \
            if (order == VisitOrder::Postorder)                       \
            visitor->visit(this);                                     \
        }                                                             \

    DECLARE_NODE_CLASS(BlockPosNode)
    DECLARE_NODE_CLASS(ColumnPosNode)
    DECLARE_NODE_CLASS(RotationNode)
    DECLARE_NODE_CLASS(Vec2Node)
    DECLARE_NODE_CLASS(Vec3Node)
}
