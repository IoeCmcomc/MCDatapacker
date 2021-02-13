#ifndef INTRANGENODE_H
#define INTRANGENODE_H

#include "argumentnode.h"
#include "integernode.h"
#include "rangenode.h"

namespace Command {
/* I hadn't gotten used to using composition yet. */
    class IntRangeNode : public ArgumentNode,
        public RangeNode<Command::IntegerNode>
    {
        Q_OBJECT

public:
        IntRangeNode(QObject *parent, int pos, int length);
        QString toString() const override;
    };
}

Q_DECLARE_METATYPE(Command::IntRangeNode*);

#endif /* INTRANGENODE_H */
