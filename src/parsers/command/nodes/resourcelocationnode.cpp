#include "resourcelocationnode.h"

const static bool _ = TypeRegister<Command::ResourceLocationNode>::init();

Command::ResourceLocationNode::ResourceLocationNode(int pos,
                                                    const QString &nspace,
                                                    const QString &id)
    : Command::ArgumentNode(pos, -1, "minecraft:resource_location") {
    setNspace(nspace);
    setId(id);
    setLength(format().length());
}

QString Command::ResourceLocationNode::toString() const {
    return QString("ResourceLocationNode(%1)").arg(format());
}

void Command::ResourceLocationNode::accept(Command::NodeVisitor *visitor,
                                           Command::NodeVisitor::Order) {
    visitor->visit(this);
}

QString Command::ResourceLocationNode::format() const {
    QString fmtStr;

    if (m_isTag)
        fmtStr += '#';
    if (m_nspace.isEmpty()) {
        fmtStr += m_id;
    } else {
        fmtStr += m_nspace + ':' + m_id;
    }
    return fmtStr;
}


QString Command::ResourceLocationNode::nspace() const {
    return m_nspace;
}

void Command::ResourceLocationNode::setNspace(const QString &nspace) {
    m_nspace = nspace;
    setLength(format().length());
}

QString Command::ResourceLocationNode::id() const {
    return m_id;
}

void Command::ResourceLocationNode::setId(const QString &id) {
    m_id = id;
    setLength(format().length());
}

QString Command::ResourceLocationNode::fullId() const {
    if (m_nspace.isEmpty()) {
        return "minecraft:" + m_id;
    } else {
        return m_nspace + ':' + m_id;
    }
}

bool Command::ResourceLocationNode::isTag() const {
    return m_isTag;
}

void Command::ResourceLocationNode::setIsTag(bool isTag) {
    m_isTag = isTag;
    setLength(format().length());
}
