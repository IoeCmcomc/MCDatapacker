#include "componentnode.h"

static bool _ = TypeRegister<Command::ComponentNode>::init();


Command::ComponentNode::ComponentNode(int pos, int length)
    : Command::ArgumentNode(pos, length, "minecraft:component") {
}

QString Command::ComponentNode::toString() const {
    return QString("ComponentNode(%1)").arg(QString::fromStdString(
                                                m_value.dump()));
}

bool Command::ComponentNode::isVaild() const {
    return ArgumentNode::isVaild() &&
           !(m_value.is_null() || m_value.is_discarded());
}

void Command::ComponentNode::accept(Command::NodeVisitor *visitor,
                                    Command::NodeVisitor::Order) {
    visitor->visit(this);
}

nlohmann::json Command::ComponentNode::value() const {
    return m_value;
}

void Command::ComponentNode::setValue(const nlohmann::json &value) {
    m_value = value;
}
