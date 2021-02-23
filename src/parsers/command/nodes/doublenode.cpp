#include "doublenode.h"

static int _ = qRegisterMetaType<Command::DoubleNode*>();

Command::DoubleNode::DoubleNode(QObject *parent, int pos, int length,
                                double value)
    : Command::ArgumentNode(parent, pos, length, "brigadier:double") {
    setValue(value);
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
