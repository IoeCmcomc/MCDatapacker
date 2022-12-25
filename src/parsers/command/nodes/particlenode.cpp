#include "particlenode.h"
#include "../visitors/nodevisitor.h"

static const int _ParticleColorNode =
    qRegisterMetaType<QSharedPointer<Command::ParticleColorNode> >();

namespace Command {
    ParticleColorNode::ParticleColorNode(int length)
        : ParseNode(Kind::Container, length) {
    }

    bool ParticleColorNode::isValid() const {
        return ParseNode::isValid() && m_r && m_g && m_b;
    }

    void ParticleColorNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        m_r->accept(visitor, order);
        m_g->accept(visitor, order);
        m_b->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    QSharedPointer<FloatNode> ParticleColorNode::b() const {
        return m_b;
    }

    void ParticleColorNode::setB(QSharedPointer<FloatNode> b) {
        m_b = b;
    }

    QSharedPointer<FloatNode> ParticleColorNode::g() const {
        return m_g;
    }

    void ParticleColorNode::setG(QSharedPointer<FloatNode> g) {
        m_g = g;
    }

    QSharedPointer<FloatNode> ParticleColorNode::r() const {
        return m_r;
    }

    void ParticleColorNode::setR(QSharedPointer<FloatNode> r) {
        m_r = r;
    }

    const static bool _ = TypeRegister<ParticleNode>::init();

    ParticleNode::ParticleNode(int length)
        : ResourceLocationNode(ParserType::Particle, length) {
    }

    ParticleNode::ParticleNode(ResourceLocationNode *other)
        : ResourceLocationNode(ParserType::Particle, other->length(),
                               other->nspace(), other->id()) {
    }

    void ParticleNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        for (const auto &param: qAsConst(m_params)) {
            param->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    ParticleNode::ParamVector ParticleNode::params() const {
        return m_params;
    }

    void ParticleNode::setParams(
        std::initializer_list<QSharedPointer<ParseNode> > params) {
        for (const auto &param: params) {
            m_params << param;
        }
    }
}
