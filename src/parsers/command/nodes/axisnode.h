#ifndef AXISNODE_H
#define AXISNODE_H

#include "parsenode.h"

namespace Command {
    class AxisNode : public ParseNode
    {
        Q_OBJECT
public:
        enum AxisType {
            Absolute,
            Relative,
            Local
        };
        union Number {
            int   intVal;
            float floatVal;
        };

        AxisNode(QObject *parent, int pos,
                 int length    = 0,
                 AxisType type = Absolute,
                 int value     = 0);
        AxisNode(QObject *parent, int pos,
                 int length    = 0,
                 AxisType type = Absolute,
                 double value  = 0);
        AxisNode(QObject *parent);

        virtual QString toString() const;
        QString format() const;

        AxisType type() const;
        void setType(const AxisType &type);

        bool useInteger() const;
        void setUseInteger(bool useInteger);

        float toFloat() const;
        int toInt() const;

        void setValue(float value);
        void setValue(int value);

protected:
        Number value() const;
        void setValue(const Number &value);

private:
        Number m_value    = Number();
        AxisType m_type   = Absolute;
        bool m_useInteger = false;
    };
}

Q_DECLARE_METATYPE(Command::AxisNode*)

#endif /* AXISNODE_H */
