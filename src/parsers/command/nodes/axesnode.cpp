#include "axesnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::AxesNode> >();

Command::AxesNode::AxesNode(int pos, int length)
    : Command::ArgumentNode(pos, length, "mcdatapacker:axes") {
}

QString Command::AxesNode::toString() const {
    QStringList components;

    if (m_x != nullptr)
        components << QString("x: %1").arg(m_x->toString());
    if (m_y != nullptr)
        components << QString("y: %1").arg(m_y->toString());
    if (m_z != nullptr)
        components << QString("z: %1").arg(m_z->toString());
    return QString("AxesNode(%1)").arg(components.join(", "));
}

bool Command::AxesNode::isVaild() const {
    return ArgumentNode::isVaild() && (m_x) && (m_z);
}

void Command::AxesNode::accept(Command::NodeVisitor *visitor) {
    if (m_x)
        m_x->accept(visitor);
    if (m_y)
        m_y->accept(visitor);
    if (m_z)
        m_z->accept(visitor);
    visitor->visit(this);
}

QSharedPointer<Command::AxisNode> Command::AxesNode::x() const {
    return m_x;
}

void Command::AxesNode::setX(QSharedPointer<AxisNode> x) {
    m_x = x;
}

QSharedPointer<Command::AxisNode> Command::AxesNode::y() {
    return m_y;
}
void Command::AxesNode::setY(QSharedPointer<AxisNode> y) {
    m_y = y;
}

QSharedPointer<Command::AxisNode> Command::AxesNode::z() const {
    return m_z;
}

void Command::AxesNode::setZ(QSharedPointer<AxisNode> z) {
    m_z = z;
}
