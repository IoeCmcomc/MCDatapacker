#ifndef ANGLENODE_H
#define ANGLENODE_H

#include "argumentnode.h"

namespace Command {
    class AngleNode : public ArgumentNode
    {
public:
        enum class AxisType : uint16_t {
            Absolute,
            Relative,
            Local
        };
        union Number {
            int   intVal;
            float floatVal;
        };

        explicit AngleNode(AxisType type = AxisType::Absolute);
        explicit AngleNode(const QString &text);

        void accept(NodeVisitor *visitor,
                    VisitOrder order)
        override;

        QString format() const;

        AxisType type() const;
        void setType(const AxisType &type);

        bool useInteger() const;
        void setUseInteger(bool useInteger);

        float toFloat() const;
        int toInt() const;

        void setValue(float value);
        void setValue(int value);

        using ParseNode::setText;

protected:
        Number value() const;
        void setValue(const Number &value);

private:
        Number m_value;
        AxisType m_type   = AxisType::Absolute;
        bool m_useInteger = false;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Angle)
}

#endif /* ANGLENODE_H */
