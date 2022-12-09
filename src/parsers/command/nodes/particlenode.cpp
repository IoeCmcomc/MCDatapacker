#include "particlenode.h"

static const int _ParticleColorNode =
    qRegisterMetaType<QSharedPointer<Command::ParticleColorNode> >();

Command::ParticleColorNode::ParticleColorNode(int pos)
    : Command::ParseNode(pos) {
}

QString Command::ParticleColorNode::toString() const {
    if (isVaild()) {
        return QString("ParticleColorNode(%1, %2, %3)").arg(
            m_r->toString(), m_g->toString(), m_b->toString());
    } else {
        return QStringLiteral("ParticleColorNode(Invaild)");
    }
}

bool Command::ParticleColorNode::isVaild() const {
    return ParseNode::isVaild() && m_r && m_g && m_b;
}

void Command::ParticleColorNode::accept(Command::NodeVisitor *visitor,
                                        Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    m_r->accept(visitor);
    m_g->accept(visitor);
    m_b->accept(visitor);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
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

const static bool _ = TypeRegister<Command::ParticleNode>::init();

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

    if (!m_params.isEmpty()) {
        ret += ", params: (";
        for (const auto &param: m_params) {
            ret += param->toString();
            if (param != m_params.last())
                ret += ", ";
        }
        ret += ")";
    }
    return ret + ')';
}

void Command::ParticleNode::accept(Command::NodeVisitor *visitor,
                                   Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    for (const auto &param: qAsConst(m_params)) {
        param->accept(visitor, order);
    }
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

Command::ParticleNode::ParamVector Command::ParticleNode::params() const {
    return m_params;
}

void Command::ParticleNode::setParams(
    std::initializer_list<QSharedPointer<ParseNode> > params) {
    for (const auto &param: params) {
        m_params << param;
    }
}

