#include "componentnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    ComponentNode::ComponentNode(const QString &text)
        : ArgumentNode(ParserType::Component, text) {
    }

    DEFINE_ACCEPT_METHOD(ComponentNode)

    nlohmann::json ComponentNode::value() const {
        return m_value;
    }

    void ComponentNode::setValue(nlohmann::json value) {
        m_isValid = !(value.is_null() || value.is_discarded());
        m_value   = std::move(value);
    }
}
