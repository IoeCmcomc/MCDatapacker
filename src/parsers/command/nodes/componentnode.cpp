#include "componentnode.h"

static const int _ = qRegisterMetaType<Command::ComponentNode*>();


Command::ComponentNode::ComponentNode(QObject *parent, int pos, int length)
    : Command::ArgumentNode(parent, pos, length, "minecraft:component") {
}

QString Command::ComponentNode::toString() const {
    return QString("ComponentNode()").arg(QString::fromStdString(m_value.dump()));
}

bool Command::ComponentNode::isVaild() const {
    return ArgumentNode::isVaild() &&
           !(m_value.is_null() || m_value.is_discarded());
}

nlohmann::json Command::ComponentNode::value() const {
    return m_value;
}

void Command::ComponentNode::setValue(const nlohmann::json &value) {
    m_value = value;
}
