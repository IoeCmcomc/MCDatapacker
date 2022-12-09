#include "uuidnode.h"

const static bool _ = TypeRegister<Command::UuidNode>::init();

Command::UuidNode::UuidNode(int pos, int length, const QString &uuidStr)
    : Command::ArgumentNode(pos, length, "minecraft:uuid"),
    m_uuid(QUuid::fromString(uuidStr)) {
}

Command::UuidNode::UuidNode(int pos, const QString &uuidStr)
    : Command::ArgumentNode(pos, uuidStr.length(), "minecraft:uuid"),
    m_uuid(QUuid::fromString(uuidStr)) {
}

QString Command::UuidNode::toString() const {
    return QString("UuidNode(%1)").arg(m_uuid.toString(QUuid::WithoutBraces));
}

bool Command::UuidNode::isVaild() const {
    return ArgumentNode::isVaild() && !m_uuid.isNull();
}

void Command::UuidNode::accept(Command::NodeVisitor *visitor,
                               Command::NodeVisitor::Order) {
    visitor->visit(this);
}

QUuid Command::UuidNode::uuid() const {
    return m_uuid;
}

void Command::UuidNode::setUuid(const QUuid &uuid) {
    m_uuid = uuid;
}
