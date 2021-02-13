#ifndef TIMENODE_H
#define TIMENODE_H

#include "argumentnode.h"

namespace Command {
    class TimeNode : public ArgumentNode
    {
        Q_OBJECT
public:
        enum Unit : unsigned char {
            Tick,
            Second,
            Day,
        };
        TimeNode(QObject *parent, int pos, int length,
                 int v, Unit unit = Unit::Tick);
        QString toString() const override;

        float value() const;
        void setValue(float value);

        Unit unit() const;
        void setUnit(const Unit &unit);

        int toTick() const;

private:
        float m_value;
        Unit m_unit = Unit::Tick;

private:
    };
}

Q_DECLARE_METATYPE(Command::TimeNode*)

#endif /* TIMENODE_H */
