#ifndef INTEGERNODE_H
#define INTEGERNODE_H

#include "argumentnode.h"

namespace Command {
    class IntegerNode : public ArgumentNode
    {
public:
        explicit IntegerNode(int pos = -1, int length = 0, int value = 0);

        QString toString() const;
        int value() const;
        void setValue(int value);

private:
        int m_value = 0;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::IntegerNode>)

#endif /* INTEGERNODE_H */
