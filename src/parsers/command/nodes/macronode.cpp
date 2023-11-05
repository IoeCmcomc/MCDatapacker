#include "macronode.h"

namespace Command {
    MacroNode::MacroNode(const int length) : ParseNode{ParseNode::Kind::Macro,
                                                       length} {
    }

    void MacroNode::append(NodePtr node) {
        m_isValid = m_segments.empty() ? node->isValid() : (m_isValid &&
                                                            node->isValid());
        m_segments.append(std::move(node));
    }

    QVector<NodePtr> MacroNode::segments() const {
        return m_segments;
    }
}
