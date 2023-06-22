#include "timenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    TimeNode::TimeNode(const QString &text, int v, Unit unit)
        : ArgumentNode(ParserType::Time, text), m_value(v), m_unit(unit) {
        m_isValid = true;
    }

    void TimeNode::accept(NodeVisitor *visitor, VisitOrder) {
        visitor->visit(this);
    }

    TimeNode::Unit TimeNode::unit() const {
        return m_unit;
    }

    void TimeNode::setUnit(const Unit &unit) {
        m_unit = unit;
    }

    int TimeNode::toTick() const {
        switch (m_unit) {
            case Unit::ImplicitTick:
            case Unit::Tick:
                return qRound(m_value);

            case Unit::Second:
                return qRound(m_value * 20);

            case Unit::Day:
                return qRound(m_value * 24000);

            default: {
                Q_UNREACHABLE();
                return -1;
            }
        }
    }

    float TimeNode::value() const {
        return m_value;
    }

    void TimeNode::setValue(float value) {
        m_value = value;
    }
}
