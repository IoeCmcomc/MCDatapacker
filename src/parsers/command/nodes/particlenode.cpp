#include "particlenode.h"
#include "../visitors/nodevisitor.h"

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
        m_b = std::move(b);
    }

    QSharedPointer<FloatNode> ParticleColorNode::g() const {
        return m_g;
    }

    void ParticleColorNode::setG(QSharedPointer<FloatNode> g) {
        m_g = std::move(g);
    }

    QSharedPointer<FloatNode> ParticleColorNode::r() const {
        return m_r;
    }

    void ParticleColorNode::setR(QSharedPointer<FloatNode> r) {
        m_r = std::move(r);
    }

    ParticleNode::ParticleNode(int length)
        : ArgumentNode(ParserType::Particle, length) {
    }

    void ParticleNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        Q_ASSERT(m_resLoc != nullptr);
        m_resLoc->accept(visitor, order);
        for (const auto &param: qAsConst(m_params)) {
            param->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    ParticleNode::ParamVector ParticleNode::params() const {
        return m_params;
    }

    QSharedPointer<ResourceLocationNode> ParticleNode::resLoc() const {
        return m_resLoc;
    }

    void ParticleNode::setResLoc(QSharedPointer<ResourceLocationNode> newResLoc)
    {
        m_resLoc = std::move(newResLoc);
    }
}
