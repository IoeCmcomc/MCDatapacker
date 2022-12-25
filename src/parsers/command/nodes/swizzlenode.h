#ifndef SWIZZLENODE_H
#define SWIZZLENODE_H

#include "argumentnode.h"

namespace Command {
    class SwizzleNode : public ArgumentNode
    {
public:
        enum class Axis : unsigned char {
            X = 1,
            Y = 2,
            Z = 4,
        };
        Q_DECLARE_FLAGS(Axes, Axis);

        SwizzleNode(const QString &text, bool hasX, bool hasY, bool hasZ);
        SwizzleNode(const QString &text, Axes axes);

        void accept(NodeVisitor *visitor, VisitOrder) override;

        Axes axes() const;
        void setAxes(const Axes &axes);

private:
        Axes m_axes;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::SwizzleNode>)
Q_DECLARE_OPERATORS_FOR_FLAGS(Command::SwizzleNode::Axes)

#endif /* SWIZZLENODE_H */
