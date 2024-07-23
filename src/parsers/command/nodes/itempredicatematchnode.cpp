#include "itempredicatematchnode.h"

#include "../visitors/nodevisitor.h"

namespace Command {
    ItemPredicateMatchNode::ItemPredicateMatchNode(NodePtr key,
                                                   const bool isPredicate)
        : PairNode<NodePtr, NodePtr> (key, nullptr),
        m_mode{Mode::MatchKey}, m_isPredicate{isPredicate} {
        m_isValid = key->isValid();
    }

    DEFINE_ACCEPT_METHOD(ItemPredicateMatchNode)

    void ItemPredicateMatchNode::setMode(Mode newMode) {
        m_mode = newMode;
    }

    ItemPredicateMatchNode::Mode ItemPredicateMatchNode::mode() const {
        return m_mode;
    }

    bool ItemPredicateMatchNode::isPredicate() const {
        return m_isPredicate;
    }
}
