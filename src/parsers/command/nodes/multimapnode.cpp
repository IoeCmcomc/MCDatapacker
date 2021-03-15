#include "multimapnode.h"

const static int _ =
    qRegisterMetaType<QSharedPointer<Command::MultiMapNode> >();


Command::MultiMapNode::MultiMapNode(int pos, int length)
    : Command::ParseNode(pos, length) {
}

QString Command::MultiMapNode::toString() const {
    QStringList itemReprs;

    for (const auto &key: m_map.uniqueKeys()) {
        QStringList valReprs;
        for (const auto &value: m_map.values(key))
            valReprs << value->toString();
        itemReprs << QString("%1: (%2)").arg(key.text, valReprs.join(", "));
    }
    return "MapNode(" + itemReprs.join("; ") + ')';
}

int Command::MultiMapNode::size() const {
    return m_map.size();
}

bool Command::MultiMapNode::contains(const Command::MapKey &key) const {
    return m_map.contains(key);
}

void Command::MultiMapNode::insert(const Command::MapKey &key,
                                   QSharedPointer<Command::ParseNode> node) {
    m_map.insert(key,
                 node);
}

void Command::MultiMapNode::replace(const Command::MapKey &key,
                                    QSharedPointer<Command::ParseNode> node) {
    m_map.replace(key, node);
}

int Command::MultiMapNode::remove(const Command::MapKey &key) {
    return m_map.remove(key);
}

void Command::MultiMapNode::clear() {
    m_map.clear();
}

QList<QSharedPointer<Command::ParseNode> > Command::MultiMapNode::values(
    const Command::MapKey &key) const {
    return m_map.values(key);
}

Command::ParseNodeMultiMap Command::MultiMapNode::toMap() const {
    return m_map;
}
