#ifndef INTEGERNODE_H
#define INTEGERNODE_H

#include "argumentnode.h"

namespace Command {
    class IntegerNode : public ArgumentNode
    {
public:
        explicit IntegerNode(QObject *parent, int pos = -1, int length = 0,
                             int value                = false);

        QString toString() const;
        int value() const;
        void setValue(int value);

private:
        int m_value;
    };
}

#endif /* INTEGERNODE_H */
