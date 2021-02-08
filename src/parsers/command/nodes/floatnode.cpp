#include "floatnode.h"

static int _ = qRegisterMetaType<Command::FloatNode*>();

Command::FloatNode::FloatNode(QObject *parent, int pos, int length,
                              float value)
    : Command::ArgumentNode(parent, pos, -1, "brigadier:float") {
    setValue(value);
    setLength(length);
}

QString Command::FloatNode::toString() const {
    return QString("FloatNode(%1)").arg(m_value);
}

float Command::FloatNode::value() const {
    return m_value;
}

void Command::FloatNode::setValue(float value) {
    m_value = value;
}
