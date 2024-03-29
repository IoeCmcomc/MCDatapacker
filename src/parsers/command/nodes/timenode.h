#ifndef TIMENODE_H
#define TIMENODE_H

#include "argumentnode.h"

namespace Command {
    class TimeNode : public ArgumentNode
    {
public:
        enum class Unit : unsigned char {
            ImplicitTick,
            Tick,
            Second,
            Day,
        };
        TimeNode(const QString &text,
                 int v, Unit unit = Unit::ImplicitTick);
        void accept(NodeVisitor *visitor, VisitOrder order) final;

        float value() const;
        void setValue(float value);

        Unit unit() const;
        void setUnit(const Unit &unit);

        int toTick() const;

private:
        float m_value = 0;
        Unit m_unit   = Unit::ImplicitTick;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Time)
}

#endif /* TIMENODE_H */
