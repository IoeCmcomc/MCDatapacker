#include "doublenode.h"

Command::DoubleNode::DoubleNode(QObject *parent, int pos, int length,
                                double value)
    : Command::ArgumentNode(parent, pos, -1, "brigadier:double") {
    setValue(value);
    setLength(length);
    setLength((value) ? 4 : 5);
}

QString Command::DoubleNode::toString() const {
    return QString("DoubleNode(%1)").arg(m_value);
}

double Command::DoubleNode::value() const {
    return m_value;
}

void Command::DoubleNode::setValue(double value) {
    m_value = value;
}
