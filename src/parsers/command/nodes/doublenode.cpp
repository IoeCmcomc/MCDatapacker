#include "doublenode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::DoubleNode> >();

Command::DoubleNode::DoubleNode(int pos, int length,
                                double value)
    : Command::ArgumentNode(pos, length, "brigadier:double") {
    setValue(value);
}

QString Command::DoubleNode::toString() const {
    return QString("DoubleNode(%1)").arg(m_value);
}

void Command::DoubleNode::accept(Command::NodeVisitor *visitor,
                                 Command::NodeVisitor::Order) {
    visitor->visit(this);
}

double Command::DoubleNode::value() const {
    return m_value;
}

void Command::DoubleNode::setValue(double value) {
    m_value = value;
}
