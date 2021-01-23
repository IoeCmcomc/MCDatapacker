#ifndef DOUBLENODE_H
#define DOUBLENODE_H

#include "argumentnode.h"

namespace Command {
    class DoubleNode : public ArgumentNode
    {
public:
        explicit DoubleNode(QObject *parent, int pos = -1, int length = 0,
                            double value             = false);

        QString toString() const;
        double value() const;
        void setValue(double value);

private:
        double m_value;
    };
}

#endif /* DOUBLENODE_H */
