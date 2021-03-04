#ifndef INTRANGENODE_H
#define INTRANGENODE_H

#include "integernode.h"
#include "rangenode.h"

namespace Command {
/* I hadn't been used to using composition yet. */
    class IntRangeNode : public RangeNode<Command::IntegerNode>
    {
public:
        IntRangeNode(int pos, int length);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::IntRangeNode>);

#endif /* INTRANGENODE_H */
