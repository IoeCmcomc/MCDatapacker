#include "uuidnode.h"

const static int _ = qRegisterMetaType<QSharedPointer<Command::UuidNode> >();

Command::UuidNode::UuidNode(int pos, QString uuidStr)
    : Command::ArgumentNode(pos, uuidStr.length(), "minecraft:uuid") {
    m_uuid = QUuid::fromString(uuidStr);
}

QString Command::UuidNode::toString() const {
    return QString("UuidNode(%1)").arg(m_uuid.toString(QUuid::WithoutBraces));
}

bool Command::UuidNode::isVaild() const {
    return ArgumentNode::isVaild() && !m_uuid.isNull();
}

QUuid Command::UuidNode::uuid() const {
    return m_uuid;
}

void Command::UuidNode::setUuid(const QUuid &uuid) {
    m_uuid = uuid;
}
