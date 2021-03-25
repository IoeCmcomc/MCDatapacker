#include "multimapnode.h"

const static int _ =
    qRegisterMetaType<QSharedPointer<Command::MultiMapNode> >();


Command::MultiMapNode::MultiMapNode(int pos, int length)
    : Command::ParseNode(pos, length) {
}

QString Command::MultiMapNode::toString() const {
    QStringList itemReprs;

    const auto &keys = m_map.uniqueKeys();

    for (auto key = keys.cbegin(); key != keys.cend(); key++) {
        /*qDebug() << key->text << m_map.values(*key).count(); */
        QStringList valReprs;
        for (const auto &val: m_map.values(*key))
            valReprs << val->toString();
        itemReprs << QString("%1: (%2)").arg(key->text, valReprs.join(", "));
    }
/*
      for (auto i = m_map.cbegin(); i != m_map.cend(); ++i)
          qDebug() << i.key().text << *i.value();
 */
    return "MultiMapNode(" + itemReprs.join("; ") + ')';
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
