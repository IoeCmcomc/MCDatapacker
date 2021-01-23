#include "literalnode.h"

Command::LiteralNode::LiteralNode(QObject *parent, const QString &txt)
    : Command::ParseNode(parent, -1, txt.length()) {
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
