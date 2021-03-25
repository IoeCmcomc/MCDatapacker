#include "stringnode.h"

const static int _ = qRegisterMetaType<QSharedPointer<Command::StringNode> >();

Command::StringNode::StringNode(int pos, const QString &value, bool isQuote)
    : Command::ArgumentNode(pos, value.length(), "brigadier:string") {
    setValue(value);
    if (isQuote)
        setLength(length() + 2);
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

QSharedPointer<Command::StringNode> Command::StringNode::fromLiteralNode(
    LiteralNode *node) {
    if (node) {
        return QSharedPointer<StringNode>::create(node->pos(), node->text());
    }
    return nullptr;
}
