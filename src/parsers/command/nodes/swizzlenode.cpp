#include "swizzlenode.h"
#include "../visitors/nodevisitor.h"

const static bool _ = TypeRegister<Command::SwizzleNode>::init();

namespace Command {
    SwizzleNode::SwizzleNode(const QString &text, bool hasX, bool hasY,
                             bool hasZ)
        : ArgumentNode(ParserType::Swizzle, text) {
        m_axes.setFlag(Axis::X, hasX);
        m_axes.setFlag(Axis::Y, hasY);
        m_axes.setFlag(Axis::Z, hasZ);

        m_isValid = int(m_axes) != 0;
    }

    SwizzleNode::SwizzleNode(const QString &text, Axes axes)
        : ArgumentNode(ParserType::Swizzle, text), m_axes(axes) {
        m_isValid = int(m_axes) != 0;
    }

    void SwizzleNode::accept(NodeVisitor *visitor,
                             VisitOrder) {
        visitor->visit(this);
    }

    SwizzleNode::Axes SwizzleNode::axes() const {
        return m_axes;
    }

    void SwizzleNode::setAxes(const Axes &axes) {
        m_axes    = axes;
        m_isValid = int(m_axes) != 0;
    }
}
