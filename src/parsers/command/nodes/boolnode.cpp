#include "boolnode.h"

Command::BoolNode::BoolNode(QObject *parent, int pos, bool value)
    : Command::ArgumentNode(parent, pos, -1, "brigadier:bool") {
    setValue(value);
    setLength((value) ? 4 : 5);
}

QString Command::BoolNode::toString() const {
    return QString("BoolNode(%1)").arg(QVariant(m_value).toString());
}

bool Command::BoolNode::value() const {
    return m_value;
}

void Command::BoolNode::setValue(bool value) {
    m_value = value;
}
