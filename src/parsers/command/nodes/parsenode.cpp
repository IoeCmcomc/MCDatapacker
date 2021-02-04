#include "parsenode.h"

Command::ParseNode::ParseNode(QObject *parent, int pos, int length) : QObject(
        parent) {
    qRegisterMetaType<Command::ParseNode*>();
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
