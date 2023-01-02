#include "floatrangenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    FloatRangeNode::FloatRangeNode(int length)
        : RangeNode(ParserType::FloatRange, length) {
    }

    void FloatRangeNode::accept(NodeVisitor *visitor, VisitOrder order) {
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
