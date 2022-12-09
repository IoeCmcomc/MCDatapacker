#include "boolnode.h"

const static bool _ = TypeRegister<Command::BoolNode>::init();

Command::BoolNode::BoolNode(int pos, bool value)
    : Command::ArgumentNode(pos, -1, "brigadier:bool") {
    setValue(value);
    setLength((value) ? 4 : 5);
}

QString Command::BoolNode::toString() const {
    return QString("BoolNode(%1)").arg(QVariant(m_value).toString());
}

void Command::BoolNode::accept(Command::NodeVisitor *visitor,
                               Command::NodeVisitor::Order) {
    visitor->visit(this);
}

bool Command::BoolNode::value() const {
    return m_value;
}

void Command::BoolNode::setValue(bool value) {
    m_value = value;
}
