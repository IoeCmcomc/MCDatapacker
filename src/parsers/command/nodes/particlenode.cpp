#include "particlenode.h"

static const int _ParticleColorNode =
    qRegisterMetaType<Command::ParticleColorNode*>();

Command::ParticleColorNode::ParticleColorNode(QObject *parent, int pos)
    : Command::ParseNode(parent, pos) {
}

QString Command::ParticleColorNode::toString() const {
    if (isVaild()) {
        return QString("ParticleColorNode(%1, %2, %3, %4)").arg(
            m_r->toString(), m_g->toString(),
            m_b->toString(), m_size->toString());
    } else {
        return "ParticleColorNode(Invaild)";
    }
}

bool Command::ParticleColorNode::isVaild() const {
    qDebug() << "ParticleColorNode::isVaild" << ParseNode::isVaild() << m_r <<
    m_g << m_b << m_size;
    return ParseNode::isVaild() && m_r && m_g && m_b && m_size;
}

Command::FloatNode *Command::ParticleColorNode::size() const {
    return m_size;
}

void Command::ParticleColorNode::setSize(FloatNode *size) {
    m_size = size;
}

Command::FloatNode *Command::ParticleColorNode::b() const {
    return m_b;
}

void Command::ParticleColorNode::setB(FloatNode *b) {
    m_b = b;
}

Command::FloatNode *Command::ParticleColorNode::g() const {
    return m_g;
}

void Command::ParticleColorNode::setG(FloatNode *g) {
    m_g = g;
}

Command::FloatNode *Command::ParticleColorNode::r() const {
    return m_r;
}

void Command::ParticleColorNode::setR(FloatNode *r) {
    m_r = r;
}

static const int _ParticleNode = qRegisterMetaType<Command::ParticleNode*>();

Command::ParticleNode::ParticleNode(QObject *parent, int pos,
                                    const QString &nspace, const QString &id)
    : Command::ResourceLocationNode(parent, pos, nspace, id) {
    setParserId("minecraft:particle");
}

Command::ParticleNode::ParticleNode(Command::ResourceLocationNode *other)
    : Command::ResourceLocationNode(other->parent(), other->pos(),
                                    other->nspace(), other->id()) {
    setParserId("minecraft:particle");
}

QString Command::ParticleNode::toString() const {
    QString ret = QString("ParticleNode(%1").arg(fullId());

    if (m_params)
        ret += ", " + m_params->toString();
    return ret + ')';
}

Command::ParseNode *Command::ParticleNode::params() const {
    return m_params;
}

void Command::ParticleNode::setParams(Command::BlockStateNode *params) {
    m_params = params;
}

void Command::ParticleNode::setParams(Command::ItemStackNode *params) {
    m_params = params;
}

void Command::ParticleNode::setParams(Command::ParticleColorNode *params) {
    m_params = params;
}
