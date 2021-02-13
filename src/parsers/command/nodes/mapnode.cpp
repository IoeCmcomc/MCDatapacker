#include "mapnode.h"

const static int _ = qRegisterMetaType<Command::MapNode*>();

Command::MapNode::MapNode(QObject *parent, int pos, int length)
    : Command::ParseNode(parent, pos, length) {
}

QString Command::MapNode::toString() const {
    QStringList itemReprs;

    for (auto i = m_map.begin(); i != m_map.end(); ++i)
        itemReprs << i.key() + ": " + i.value()->toString();
    return "MapNode(" + itemReprs.join(", ") + ')';
}

int Command::MapNode::size() const {
    return m_map.size();
}

bool Command::MapNode::contains(const QString &key) const {
    return m_map.contains(key);
}

void Command::MapNode::insert(const QString &key, Command::ParseNode *node) {
    m_map.insert(key, node);
}

int Command::MapNode::remove(const QString &key) {
    return m_map.remove(key);
}
void Command::MapNode::clear() {
    m_map.clear();
}

Command::ParseNode* &Command::MapNode::operator[](const QString &key) {
    return m_map[key];
}

Command::ParseNode* Command::MapNode::operator[](const QString &key) const {
    return m_map[key];
}
