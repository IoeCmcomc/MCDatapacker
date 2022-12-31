#ifndef FLOATRANGENODE_H
#define FLOATRANGENODE_H

#include "singlevaluenode.h"
#include "rangenode.h"

namespace Command {
    class FloatRangeNode : public RangeNode<FloatNode>
    {
public:
        explicit FloatRangeNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, FloatRange)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::FloatRangeNode>);
#endif /* FLOATRANGENODE_H */
