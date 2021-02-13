#ifndef INTEGERNODE_H
#define INTEGERNODE_H

#include "argumentnode.h"

namespace Command {
    class IntegerNode : public ArgumentNode
    {
        Q_OBJECT
public:
        explicit IntegerNode(QObject *parent, int pos = -1, int length = 0,
                             int value                = 0);

        QString toString() const;
        int value() const;
        void setValue(int value);

private:
        int m_value;
    };
}

Q_DECLARE_METATYPE(Command::IntegerNode*)

#endif /* INTEGERNODE_H */
