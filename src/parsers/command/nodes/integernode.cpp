#include "integernode.h"

const static bool _ = TypeRegister<Command::IntegerNode>::init();

Command::IntegerNode::IntegerNode(int pos, int length, int value)
    : Command::ArgumentNode(pos, length, "brigadier:integer") {
    setValue(value);
}

QString Command::IntegerNode::toString() const {
    return QString("IntegerNode(%1)").arg(m_value);
}

void Command::IntegerNode::accept(Command::NodeVisitor *visitor,
                                  Command::NodeVisitor::Order) {
    visitor->visit(this);
}

int Command::IntegerNode::value() const {
    return m_value;
}

void Command::IntegerNode::setValue(int value) {
    m_value = value;
}

