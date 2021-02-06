#ifndef AXESNODE_H
#define AXESNODE_H

#include "argumentnode.h"
#include "axisnode.h"

namespace Command {
    class AxesNode : public ArgumentNode /* Axes, as the plural form of 'axis' */
    {
        Q_OBJECT
public:
        explicit AxesNode(QObject *parent, int pos = -1,
                          int length               = 0);
        virtual QString toString() const;
        bool isVaild() const;

        AxisNode *y();
        void setY(AxisNode *y);

        AxisNode *x() const;
        void setX(AxisNode *x);

        AxisNode *z() const;
        void setZ(AxisNode *z);

private:
        AxisNode *m_x = nullptr;
        AxisNode *m_z = nullptr;
        AxisNode *m_y = nullptr;
    };
}

Q_DECLARE_METATYPE(Command::AxesNode*)

#endif /* AXESNODE_H */
