#include "stylenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    StyleNode::StyleNode(const QString &text)
        : ArgumentNode(ParserType::Component, text) {
    }

    DEFINE_ACCEPT_METHOD(StyleNode)

    nlohmann::json StyleNode::value() const {
        return m_value;
    }

    void StyleNode::setValue(nlohmann::json value) {
        m_isValid = !(value.is_null() || value.is_discarded());
        m_value   = std::move(value);
    }
}
