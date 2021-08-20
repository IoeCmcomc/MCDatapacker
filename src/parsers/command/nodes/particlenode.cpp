#include "particlenode.h"

static const int _ParticleColorNode =
    qRegisterMetaType<QSharedPointer<Command::ParticleColorNode> >();

Command::ParticleColorNode::ParticleColorNode(int pos)
    : Command::ParseNode(pos) {
}

QString Command::ParticleColorNode::toString() const {
    if (isVaild()) {
        return QString("ParticleColorNode(%1, %2, %3, %4)").arg(
            m_r->toString(), m_g->toString(),
            m_b->toString(), m_size->toString());
    } else {
        return QStringLiteral("ParticleColorNode(Invaild)");
    }
}

bool Command::ParticleColorNode::isVaild() const {
    return ParseNode::isVaild() && m_r && m_g && m_b && m_size;
}

void Command::ParticleColorNode::accept(Command::NodeVisitor *visitor,
                                        Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    m_r->accept(visitor);
    m_g->accept(visitor);
    m_b->accept(visitor);
    m_size->accept(visitor);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

QSharedPointer<Command::FloatNode> Command::ParticleColorNode::size() const {
    return m_size;
}

void Command::ParticleColorNode::setSize(QSharedPointer<FloatNode> size) {
    m_size = size;
}

QSharedPointer<Command::FloatNode> Command::ParticleColorNode::b() const {
    return m_b;
}

void Command::ParticleColorNode::setB(QSharedPointer<FloatNode> b) {
    m_b = b;
}

QSharedPointer<Command::FloatNode> Command::ParticleColorNode::g() const {
    return m_g;
}

void Command::ParticleColorNode::setG(QSharedPointer<FloatNode> g) {
    m_g = g;
}

QSharedPointer<Command::FloatNode> Command::ParticleColorNode::r() const {
    return m_r;
}

void Command::ParticleColorNode::setR(QSharedPointer<FloatNode> r) {
    m_r = r;
}

static const int _ParticleNode =
    qRegisterMetaType<QSharedPointer<Command::ParticleNode> >();

Command::ParticleNode::ParticleNode(int pos, const QString &nspace,
                                    const QString &id)
    : Command::ResourceLocationNode(pos, nspace, id) {
    setParserId("minecraft:particle");
}

Command::ParticleNode::ParticleNode(Command::ResourceLocationNode *other)
    : Command::ResourceLocationNode(other->pos(), other->nspace(),
                                    other->id()) {
    setParserId("minecraft:particle");
}

QString Command::ParticleNode::toString() const {
    QString ret = QString("ParticleNode(%1").arg(fullId());

    if (m_params)
        ret += QStringLiteral(", ") + m_params->toString();
    return ret + ')';
}

void Command::ParticleNode::accept(Command::NodeVisitor *visitor,
                                   Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (m_params)
        m_params->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

QSharedPointer<Command::ParseNode> Command::ParticleNode::params() const {
    return m_params;
}

void Command::ParticleNode::setParams(QSharedPointer<BlockStateNode> params) {
    m_params = params;
}

void Command::ParticleNode::setParams(QSharedPointer<ItemStackNode> params) {
    m_params = params;
}

void Command::ParticleNode::setParams(QSharedPointer<ParticleColorNode> params)
{
    m_params = params;
}
