#include "blockstatenode.h"

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

Command::MapNode *Command::BlockStateNode::nbt() const {
    return m_nbt;
}

void Command::BlockStateNode::setNbt(MapNode *nbt) {
    m_nbt = nbt;
}

Command::MapNode *Command::BlockStateNode::states() const {
    return m_states;
}

void Command::BlockStateNode::setStates(MapNode *states) {
    m_states = states;
}
