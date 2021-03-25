#include "parsenode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::ParseNode> >();

Command::ParseNode::ParseNode(int pos, int length) {
    setPos(pos);
    setLength(length);
}

QString Command::ParseNode::toString() const {
    return "ParseNode()";
}

bool Command::ParseNode::isVaild() const {
    return pos() > -1;
}

void Command::ParseNode::setLength(int length) {
    m_length = length;
}

void Command::ParseNode::accept(Command::NodeVisitor *visitor) {
    visitor->visit(this);
}

int Command::ParseNode::length() const {
    return m_length;
}

int Command::ParseNode::pos() const {
    return m_pos;
}

void Command::ParseNode::setPos(int pos) {
    m_pos = pos;
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node) {
    QDebugStateSaver saver(debug);

    debug.nospace() << node.toString();
    return debug;
}
