#ifndef DOUBLENODE_H
#define DOUBLENODE_H

#include "argumentnode.h"

namespace Command {
    class DoubleNode : public ArgumentNode
    {
public:
        explicit DoubleNode(int pos = -1, int length = 0, double value = 0);

        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }
        double value() const;
        void setValue(double value);

private:
        double m_value = 0;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::DoubleNode>);

#endif /* DOUBLENODE_H */
