#ifndef FLOATRANGENODE_H
#define FLOATRANGENODE_H

#include "floatnode.h"
#include "rangenode.h"

namespace Command {
    class FloatRangeNode : public RangeNode<Command::FloatNode>
    {
public:
        FloatRangeNode(int pos, int length);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::FloatRangeNode>);
#endif /* FLOATRANGENODE_H */
