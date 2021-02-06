#include "axesnode.h"

Command::AxesNode::AxesNode(QObject *parent, int pos, int length)
    : Command::ArgumentNode(parent, pos, length, "mcdatapacker:axes") {
    qMetaTypeId<Command::AxesNode*>();
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

Command::AxisNode *Command::AxesNode::x() const {
    return m_x;
}

void Command::AxesNode::setX(AxisNode *x) {
    x->setParent(this);
    m_x = x;
}

Command::AxisNode *Command::AxesNode::y() {
    return m_y;
}
void Command::AxesNode::setY(AxisNode *y) {
    y->setParent(this);
    m_y = y;
}

Command::AxisNode *Command::AxesNode::z() const {
    return m_z;
}

void Command::AxesNode::setZ(AxisNode *z) {
    z->setParent(this);
    m_z = z;
}
