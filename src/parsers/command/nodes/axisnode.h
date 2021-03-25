#ifndef AXISNODE_H
#define AXISNODE_H

#include "parsenode.h"

namespace Command {
    class AxisNode : public ParseNode
    {
public:
        enum AxisType : unsigned char {
            Absolute,
            Relative,
            Local
        };
        union Number {
            int   intVal;
            float floatVal;
        };

        AxisNode(int pos, int length, AxisType type = Absolute,
                 int value                          = 0);
        AxisNode(int pos, int length, AxisType type = Absolute,
                 double value                       = 0);
        AxisNode(int pos);

        virtual QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }
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
        Number m_value;
        AxisType m_type   = Absolute;
        bool m_useInteger = false;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::AxisNode>)

#endif /* AXISNODE_H */
