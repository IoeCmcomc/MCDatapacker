#include "componentnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    ComponentNode::ComponentNode(const QString &text)
        : ArgumentNode(ParserType::Component, text) {
    }

    bool ComponentNode::isValid() const {
        return ArgumentNode::isValid() &&
               !(m_value.is_null() || m_value.is_discarded());
    }

    DEFINE_ACCEPT_METHOD(ComponentNode)

    nlohmann::json ComponentNode::value() const {
        return m_value;
    }

    void ComponentNode::setValue(const nlohmann::json &value) {
        m_value = value;
    }
}
