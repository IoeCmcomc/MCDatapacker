#include "resourcelocationnode.h"

Command::ResourceLocationNode::ResourceLocationNode(QObject *parent,
                                                    int pos,
                                                    const QString &nspace,
                                                    const QString &id)
    : Command::ArgumentNode(parent,
                            pos,
                            nspace.length() + id.length() + 1,
                            "minecraft:resource_location") {
    qMetaTypeId<ResourceLocationNode*>();
    setNspace(nspace);
    setId(id);
}

QString Command::ResourceLocationNode::toString() const {
    return QString("ResourceLocationNode(%1:%2)").arg(m_nspace, m_id);
}

QString Command::ResourceLocationNode::nspace() const {
    return m_nspace;
}

void Command::ResourceLocationNode::setNspace(const QString &nspace) {
    m_nspace = nspace;
}

QString Command::ResourceLocationNode::id() const {
    return m_id;
}

void Command::ResourceLocationNode::setId(const QString &id) {
    m_id = id;
}

QString Command::ResourceLocationNode::fullId() const {
    return m_nspace + ':' + m_id;
}

bool Command::ResourceLocationNode::isTag() const {
    return m_isTag;
}

void Command::ResourceLocationNode::setIsTag(bool isTag) {
    m_isTag = isTag;
}
