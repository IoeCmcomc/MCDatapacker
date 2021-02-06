#include "axisnode.h"

Command::AxisNode::AxisNode(QObject *parent,
                            int pos,
                            int length,
                            Command::AxisNode::AxisType type,
                            double value)
    : Command::ParseNode(parent, pos, length) {
    setValue((float)value);
    setType(type);
}

Command::AxisNode::AxisNode(QObject *parent)
    : Command::ParseNode(this) {
}

Command::AxisNode::AxisNode(QObject *parent,
                            int pos,
                            int length,
                            Command::AxisNode::AxisType type,
                            int value)
    : Command::ParseNode(parent, pos, length) {
    setValue(value);
    setType(type);
}


QString Command::AxisNode::toString() const {
    return QString("AxisNode(%1)").arg(format());
}

QString Command::AxisNode::format() const {
    QString type2char[] = { "", "~", "^" };
    QString number      =
        (m_useInteger) ? QString::number(toInt()) : QString::number(toFloat());

    return type2char[m_type] + number;
}

bool Command::AxisNode::useInteger() const {
    return m_useInteger;
}

void Command::AxisNode::setUseInteger(bool useInteger) {
    m_useInteger = useInteger;
}

float Command::AxisNode::toFloat() const {
    return (!m_useInteger) ? m_value.floatVal : 0;
}

int Command::AxisNode::toInt() const {
    return (m_useInteger) ? m_value.intVal : 0;
}

void Command::AxisNode::setValue(float value) {
    m_value.floatVal = value;
    m_useInteger     = false;
}

void Command::AxisNode::setValue(int value) {
    m_value.intVal = value;
    m_useInteger   = true;
}

Command::AxisNode::AxisType Command::AxisNode::type() const {
    return m_type;
}

void Command::AxisNode::setType(const AxisType &type) {
    m_type = type;
}

Command::AxisNode::Number Command::AxisNode::value() const {
    return m_value;
}

void Command::AxisNode::setValue(const Number &value) {
    m_value = value;
}
