#include "floatnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::FloatNode> >();

Command::FloatNode::FloatNode(int pos, int length, float value)
    : Command::ArgumentNode(pos, length, "brigadier:float") {
    setValue(value);
}

QString Command::FloatNode::toString() const {
    return QString("FloatNode(%1)").arg(m_value);
}

void Command::FloatNode::accept(Command::NodeVisitor *visitor,
                                Command::NodeVisitor::Order) {
    visitor->visit(this);
}

float Command::FloatNode::value() const {
    return m_value;
}

void Command::FloatNode::setValue(float value) {
    m_value = value;
}
