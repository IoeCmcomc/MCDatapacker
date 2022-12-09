#include "timenode.h"

static bool _ = TypeRegister<Command::TimeNode>::init();

Command::TimeNode::TimeNode(int pos, int length, int v, Unit unit)
    : Command::ArgumentNode(pos, length, "minecraf:time") {
    setValue(v);
    setUnit(unit);
}

QString Command::TimeNode::toString() const {
    const QString type2char[] = { "", "t", "s", "d" };

    return QString("TimeNode(%1)").arg(QString::number(m_value) +
                                       type2char[m_unit]);
}

void Command::TimeNode::accept(Command::NodeVisitor *visitor,
                               Command::NodeVisitor::Order) {
    visitor->visit(this);
}

Command::TimeNode::Unit Command::TimeNode::unit() const {
    return m_unit;
}

void Command::TimeNode::setUnit(const Unit &unit) {
    if (unit > 3) {
        qWarning() << "Invaild unit type";
        return;
    }
    m_unit = unit;
}

int Command::TimeNode::toTick() const {
    switch (m_unit) {
        case Unit::ImplicitTick:
        case Unit::Tick:
            return qRound(m_value);

        case Unit::Second:
            return qRound(m_value * 20);

        case Unit::Day:
            return qRound(m_value * 24000);

        default: {
            qWarning() << "Unknown unit: " << m_unit << ". Return -1.";
            return -1;
        }
    }
}

float Command::TimeNode::value() const {
    return m_value;
}

void Command::TimeNode::setValue(float value) {
    m_value = value;
}
