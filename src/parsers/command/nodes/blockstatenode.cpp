#include "blockstatenode.h"

static const int _ = qRegisterMetaType<Command::BlockStateNode*>();

Command::BlockStateNode::BlockStateNode(QObject *parent,
                                        int pos,
                                        const QString &nspace,
                                        const QString &id)
    : Command::ResourceLocationNode(parent, pos, nspace, id) {
}

QString Command::BlockStateNode::toString() const {
    auto ret =
        QString("BlockStateNode(%1)").arg(ResourceLocationNode::fullId());

    if (m_states)
        ret += '[' + m_states->toString() + ']';
    if (m_nbt)
        ret += '{' + m_nbt->toString() + '}';
    return ret;
}

bool Command::BlockStateNode::isVaild() const {
    return ResourceLocationNode::isVaild() && m_states && m_nbt;
}


Command::MapNode *Command::BlockStateNode::states() const {
    return m_states;
}

void Command::BlockStateNode::setStates(MapNode *states) {
    m_states = states;
}

Command::NbtCompoundNode *Command::BlockStateNode::nbt() const {
    return m_nbt;
}

void Command::BlockStateNode::setNbt(NbtCompoundNode *nbt) {
    m_nbt = nbt;
}

Command::BlockPredicateNode::BlockPredicateNode(QObject *parent,
                                                int pos,
                                                const QString &nspace,
                                                const QString &id)
    : Command::BlockStateNode(parent, pos, nspace, id) {
    setParserId("minecraft:block_predicate");
}

Command::BlockPredicateNode::BlockPredicateNode(Command::BlockStateNode *other)
    : Command::BlockStateNode(other->parent(), other->pos(),
                              other->nspace(), other->id()) {
    setParserId("minecraft:block_predicate");
    setLength(other->length());
}

QString Command::BlockPredicateNode::toString() const {
    return BlockStateNode::toString().replace(0, 13, "BlockPredicateNode");
}
