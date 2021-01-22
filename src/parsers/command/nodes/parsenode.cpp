#include "parsenode.h"

Command::ParseNode::ParseNode(QObject *parent) : QObject(parent) {
}

QString Command::ParseNode::toString() const {
    return "ParseNode()";
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node) {
    QDebugStateSaver saver(debug);

    debug.nospace() << node.toString();
    return debug;
}
