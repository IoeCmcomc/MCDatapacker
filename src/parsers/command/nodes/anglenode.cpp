#include "anglenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    AngleNode::AngleNode(AngleNode::AxisType type)
        : ArgumentNode(ParserType::Angle, QString()), m_type{type} {
    }

    AngleNode::AngleNode(const QString &text)
        : ArgumentNode(ParserType::Angle, text) {
        setValue(0);
    }

    void AngleNode::accept(NodeVisitor *visitor,
                           VisitOrder) {
        visitor->visit(this);
    }

    bool AngleNode::useInteger() const {
        return m_useInteger;
    }

    void AngleNode::setUseInteger(bool useInteger) {
        m_useInteger = useInteger;
    }

    float AngleNode::toFloat() const {
        return (!m_useInteger) ? m_value.floatVal : 0;
    }

    int AngleNode::toInt() const {
        return (m_useInteger) ? m_value.intVal : 0;
    }

    void AngleNode::setValue(float value) {
        m_value.floatVal = value;
        m_useInteger     = false;
    }

    void AngleNode::setValue(int value) {
        m_value.intVal = value;
        m_useInteger   = true;
    }

    AngleNode::AxisType AngleNode::type() const {
        return m_type;
    }

    void AngleNode::setType(const AxisType &type) {
        m_type = type;
    }

    AngleNode::Number AngleNode::value() const {
        return m_value;
    }

    void AngleNode::setValue(const Number &value) {
        m_value = value;
    }
}
