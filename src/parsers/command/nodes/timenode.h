#ifndef TIMENODE_H
#define TIMENODE_H

#include "argumentnode.h"

namespace Command {
    class TimeNode : public ArgumentNode
    {
public:
        enum Unit : unsigned char {
            ImplicitTick,
            Tick,
            Second,
            Day,
        };
        TimeNode(int pos, int length,
                 int v, Unit unit = Unit::ImplicitTick);
        QString toString() const override;

        float value() const;
        void setValue(float value);

        Unit unit() const;
        void setUnit(const Unit &unit);

        int toTick() const;

private:
        float m_value;
        Unit m_unit = Unit::ImplicitTick;

private:
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::TimeNode>)

#endif /* TIMENODE_H */
