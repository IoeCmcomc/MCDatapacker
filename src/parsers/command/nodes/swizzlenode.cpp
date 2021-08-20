#include "swizzlenode.h"

const static int _ = qRegisterMetaType<QSharedPointer<Command::SwizzleNode> >();

Command::SwizzleNode::SwizzleNode(int pos, bool hasX,
                                  bool hasY, bool hasZ)
    : ArgumentNode(pos, 0, "minecraft:swizzle") {
    m_axes.setFlag(Axis::X, hasX);
    m_axes.setFlag(Axis::Y, hasY);
    m_axes.setFlag(Axis::Z, hasZ);

    update();
}

Command::SwizzleNode::SwizzleNode(int pos, Axes axes)
    : ArgumentNode(pos, 0, "minecraft:swizzle") {
    setAxes(axes);
}

QString Command::SwizzleNode::toString() const {
    QString ret = "SwizzleNode(";

    if (m_axes & Axis::X)
        ret += 'x';
    if (m_axes & Axis::Y)
        ret += 'y';
    if (m_axes & Axis::Z)
        ret += 'z';
    return ret + ')';
}

bool Command::SwizzleNode::isVaild() const {
    return ArgumentNode::isVaild() && (length() > 0);
}

void Command::SwizzleNode::accept(Command::NodeVisitor *visitor,
                                  Command::NodeVisitor::Order) {
    visitor->visit(this);
}

Command::SwizzleNode::Axes Command::SwizzleNode::axes() const {
    return m_axes;
}

void Command::SwizzleNode::setAxes(const Axes &axes) {
    m_axes = axes;
    update();
}

void Command::SwizzleNode::update() {
    int len = 0;

    if (m_axes & Axis::X)
        len += 1;
    if (m_axes & Axis::Y)
        len += 1;
    if (m_axes & Axis::Z)
        len += 1;
    setLength(len);
}
