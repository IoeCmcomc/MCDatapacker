#ifndef ITEMPREDICATEMATCHNODE_H
#define ITEMPREDICATEMATCHNODE_H

#include "mapnode.h"

namespace Command {
    /*!
     * \class ItemPredicateMatchNode
     * \brief Represent either a component in an item stack or a component test
     * in an item predicate.
     *
     */
    class ItemPredicateMatchNode : public PairNode<NodePtr, NodePtr>
    {
public:
        enum class Mode: uchar {
            FullMatch,
            PartialMatch,
            MatchKey,
        };

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        ItemPredicateMatchNode(NodePtr key, const bool isPredicate = true);

        void setMode(Mode newMode);
        Mode mode() const;

        bool isPredicate() const;

private:
        Mode m_mode        = Mode::FullMatch;
        bool m_isPredicate = true;
    };
}

#endif // ITEMPREDICATEMATCHNODE_H
