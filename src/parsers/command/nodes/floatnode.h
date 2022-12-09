
#ifndef FLOATNODE_H
#define FLOATNODE_H

#include "argumentnode.h"

namespace Command {
    class FloatNode : public ArgumentNode
    {
public:
        explicit FloatNode(int pos = -1, int length = 0, float value = 0);

        QString toString() const override;
        void accept(NodeVisitor *visitor,
                    NodeVisitor::Order order = NodeVisitor::Order::Postorder)
        override;
        float value() const;
        void setValue(float value);

private:
        float m_value = 0;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::FloatNode>);

#endif /* FLOATNODE_H */
