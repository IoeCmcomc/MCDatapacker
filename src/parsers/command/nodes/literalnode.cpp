#include "literalnode.h"

Command::LiteralNode::LiteralNode(QObject *parent, int pos, const QString &txt)
    : Command::ParseNode(parent, pos, txt.length()) {
    qRegisterMetaType<Command::LiteralNode*>();
    setText(txt);
}

QString Command::LiteralNode::toString() const {
    return QString("LiteralNode(%1)").arg(text());
}

QString Command::LiteralNode::text() const {
    return m_text;
}

void Command::LiteralNode::setText(const QString &text) {
    m_text = text;
}

Command::StringNode *Command::LiteralNode::toStringNode(bool autoDelete) {
    auto *ret = new Command::StringNode(parent(), pos(), text());

    if (autoDelete)
        deleteLater();
    return ret;
}

