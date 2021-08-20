#include "argumentnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::ArgumentNode> >();

Command::ArgumentNode::ArgumentNode(int pos, int length, QString parserId)
    : Command::ParseNode(pos, length) {
    setParserId(parserId);
}

QString Command::ArgumentNode::toString() const {
    return QString("ArgumentNode<%1>").arg(parserId());
}

QString Command::ArgumentNode::parserId() const {
    return m_parserId;
}

void Command::ArgumentNode::accept(Command::NodeVisitor *visitor,
                                   Command::NodeVisitor::Order) {
    visitor->visit(this);
}

void Command::ArgumentNode::setParserId(const QString &parserId) {
    m_parserId = parserId;
}
