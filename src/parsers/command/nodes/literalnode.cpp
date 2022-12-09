#include "literalnode.h"

const static bool _ = TypeRegister<Command::LiteralNode>::init();

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

void Command::LiteralNode::accept(Command::NodeVisitor *visitor,
                                  Command::NodeVisitor::Order) {
    visitor->visit(this);
}

bool Command::LiteralNode::isCommand() const {
    return m_isCommand;
}

void Command::LiteralNode::setIsCommand(bool isCommand) {
    m_isCommand = isCommand;
}
