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
        const auto &keys = m_map.values(*key);
        for (const auto &val: qAsConst(keys))
            valReprs << val->toString();
        itemReprs << QString("%1: (%2)").arg(key->text, valReprs.join(", "));
    }
/*
      for (auto i = m_map.cbegin(); i != m_map.cend(); ++i)
          qDebug() << i.key().text << *i.value();
 */
    return "MultiMapNode(" + itemReprs.join("; ") + ')';
}

void Command::MultiMapNode::accept(Command::NodeVisitor *visitor,
                                   Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);

    const auto &&keys = m_map.uniqueKeys();
    for (const auto &key: qAsConst(keys)) {
        visitor->visit(key);
        const auto &values = m_map.values(key);
        for (const auto &value: qAsConst(values))
            value->accept(visitor, order);
    }
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
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
