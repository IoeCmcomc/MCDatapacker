#include "boolnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::BoolNode> >();

Command::BoolNode::BoolNode(int pos, bool value)
    : Command::ArgumentNode(pos, -1, "brigadier:bool") {
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
