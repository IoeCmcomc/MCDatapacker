#include "mapnode.h"
#include <tuple>

bool Command::MapKey::operator<(const Command::MapKey &other) const {
    return std::tie(pos, text) < std::tie(other.pos, other.text);
}

const static int _ = qRegisterMetaType<Command::MapNode*>();

Command::MapNode::MapNode(QObject *parent, int pos, int length)
    : Command::ParseNode(parent, pos, length) {
}

QString Command::MapNode::toString() const {
    QStringList itemReprs;

    for (auto i = m_map.cbegin(); i != m_map.cend(); ++i)
        itemReprs << QString("%1: %2").arg(i.key().text,
                                           i.value()->toString());
    return "MapNode(" + itemReprs.join(", ") + ')';
}

int Command::MapNode::size() const {
    return m_map.size();
}

bool Command::MapNode::contains(const QString &key) const {
    auto it = m_map.cbegin();

    while (it != m_map.cend()) {
        if (it.key().text == key)
            return true;

        ++it;
    }
    return false;
}

bool Command::MapNode::contains(const MapKey &key)
const {
    return m_map.contains(key);
}

Command::ParseNodeMap::const_iterator Command::MapNode::find(const QString &key)
const {
    auto it = m_map.cbegin();

    while (it != m_map.cend()) {
        if (it.key().text == key)
            return it;

        ++it;
    }
    return m_map.cend();
}

void Command::MapNode::insert(const MapKey &key,
                              Command::ParseNode *node) {
    m_map.insert(key, node);
}

int Command::MapNode::remove(const MapKey &key) {
    return m_map.remove(key);
}
void Command::MapNode::clear() {
    m_map.clear();
}

Command::ParseNode *&Command::MapNode::operator[](const Command::MapKey &key) {
    return m_map[key];
}

Command::ParseNode *Command::MapNode::operator[](const Command::MapKey &key)
const {
    return m_map[key];
}

Command::ParseNodeMap Command::MapNode::toMap() const {
    return m_map;
}
