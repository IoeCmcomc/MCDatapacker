#ifndef INTRANGENODE_H
#define INTRANGENODE_H

#include "singlevaluenode.h"
#include "rangenode.h"

namespace Command {
    class IntRangeNode : public RangeNode<IntegerNode> {
public:
        explicit IntRangeNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, IntRange)
}

#endif /* INTRANGENODE_H */
