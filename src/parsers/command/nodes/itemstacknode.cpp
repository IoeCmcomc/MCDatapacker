#include "itemstacknode.h"

static const int _ItemStackNode = qRegisterMetaType<Command::ItemStackNode*>();

Command::ItemStackNode::ItemStackNode(QObject *parent, int pos,
                                      const QString &nspace,
                                      const QString &id)
    : Command::ResourceLocationNode(parent, pos, nspace, id) {
}

QString Command::ItemStackNode::toString() const {
    auto ret =
        QString("ItemStackNode(%1)").arg(ResourceLocationNode::fullId());

    if (m_nbt)
        ret += '{' + m_nbt->toString() + '}';
    return ret;
}

bool Command::ItemStackNode::isVaild() const {
    return ResourceLocationNode::isVaild() && m_nbt;
}

Command::NbtCompoundNode *Command::ItemStackNode::nbt() const {
    return m_nbt;
}

void Command::ItemStackNode::setNbt(NbtCompoundNode *nbt) {
    m_nbt = nbt;
}

static const int _ItemPredicateNode =
    qRegisterMetaType<Command::ItemPredicateNode*>();

Command::ItemPredicateNode::ItemPredicateNode(QObject *parent, int pos,
                                              const QString &nspace,
                                              const QString &id)
    : Command::ItemStackNode(parent, pos, nspace, id) {
    setParserId("minecraft:item_predicate");
}

Command::ItemPredicateNode::ItemPredicateNode(Command::ItemStackNode *other)
    : Command::ItemStackNode(other->parent(), other->pos(),
                             other->nspace(), other->id()) {
    setParserId("minecraft:item_predicate");
    setLength(other->length());
}

QString Command::ItemPredicateNode::toString() const {
    return ItemStackNode::toString().replace(0, 12, "ItemPredicateNode");
}
