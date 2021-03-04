#include "literalnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::LiteralNode> >();

Command::LiteralNode::LiteralNode(int pos, const QString &txt)
    : Command::ParseNode(pos, txt.length()) {
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
