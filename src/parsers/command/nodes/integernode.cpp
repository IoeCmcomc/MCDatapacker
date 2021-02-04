#include "integernode.h"

Command::IntegerNode::IntegerNode(QObject *parent, int pos, int length,
                                  int value)
    : Command::ArgumentNode(parent, pos, -1, "brigadier:integer") {
    qRegisterMetaType<Command::IntegerNode*>();
    setValue(value);
    setLength(length);
}

QString Command::IntegerNode::toString() const {
    return QString("IntegerNode(%1)").arg(m_value);
}

int Command::IntegerNode::value() const {
    return m_value;
}

void Command::IntegerNode::setValue(int value) {
    m_value = value;
}

