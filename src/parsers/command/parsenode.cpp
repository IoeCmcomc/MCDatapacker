#include "parsenode.h"

#include <QDebug>

CommandParser::ParseNode::ParseNode(QObject *parent) : QObject(parent) {
}

QString CommandParser::ParseNode::toString() const {
    return "ParseNode()";
}

QDebug operator<<(QDebug debug, const CommandParser::ParseNode &node) {
    QDebugStateSaver saver(debug);

    debug.nospace() << node.toString();
    return debug;
}
