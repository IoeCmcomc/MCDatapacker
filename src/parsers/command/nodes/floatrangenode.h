#ifndef FLOATRANGENODE_H
#define FLOATRANGENODE_H

#include "argumentnode.h"
#include "floatnode.h"
#include "rangenode.h"

namespace Command {
    class FloatRangeNode : public ArgumentNode,
        public RangeNode<Command::FloatNode>
    {
        Q_OBJECT

public:
        FloatRangeNode(QObject *parent, int pos, int length);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(Command::FloatRangeNode*);
#endif /* FLOATRANGENODE_H */
