#ifndef AXESNODE_H
#define AXESNODE_H

#include "argumentnode.h"
#include "axisnode.h"

namespace Command {
    class AxesNode : public ArgumentNode /* Axes, as the plural form of 'axis' */
    {
public:
        explicit AxesNode(int pos = -1, int length = 0);
        virtual QString toString() const override;
        bool isVaild() const override;
        virtual void accept(NodeVisitor *visitor) override;

        QSharedPointer<AxisNode> y();
        void setY(QSharedPointer<AxisNode> y);

        QSharedPointer<AxisNode> x() const;
        void setX(QSharedPointer<AxisNode> x);

        QSharedPointer<AxisNode> z() const;
        void setZ(QSharedPointer<AxisNode> z);

private:
        QSharedPointer<AxisNode> m_x = nullptr;
        QSharedPointer<AxisNode> m_z = nullptr;
        QSharedPointer<AxisNode> m_y = nullptr;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::AxesNode>)

#endif /* AXESNODE_H */
