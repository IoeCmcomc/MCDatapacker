#include "stringnode.h"

const static int _ = qRegisterMetaType<Command::StringNode*>();

Command::StringNode::StringNode(QObject *parent, int pos, const QString &value)
    : Command::ArgumentNode(parent, pos, value.length(), "brigadier:string") {
    setValue(value);
}

QString Command::StringNode::toString() const {
    return QString("StringNode(\"%1\")").arg(m_value);
}

QString Command::StringNode::value() const {
    return m_value;
}

void Command::StringNode::setValue(const QString &value) {
    m_value = value;
    setLength(value.length());
}
