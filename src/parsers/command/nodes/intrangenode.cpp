#include "intrangenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    IntRangeNode::IntRangeNode(int length)
        : RangeNode(ParserType::IntRange, length) {
    }

    void IntRangeNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        primary()->accept(visitor, order);
        if (secondary())
            secondary()->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
}
