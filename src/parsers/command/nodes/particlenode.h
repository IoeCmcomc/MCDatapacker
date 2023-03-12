#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "singlevaluenode.h"
#include "resourcelocationnode.h"

namespace Command {
    class ParticleColorNode : public ParseNode {
public:
        explicit ParticleColorNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<FloatNode> r() const;
        void setR(QSharedPointer<FloatNode> r);

        QSharedPointer<FloatNode> g() const;
        void setG(QSharedPointer<FloatNode> g);

        QSharedPointer<FloatNode> b() const;
        void setB(QSharedPointer<FloatNode> b);

private:
        QSharedPointer<FloatNode> m_r;
        QSharedPointer<FloatNode> m_g;
        QSharedPointer<FloatNode> m_b;
    };

    class ParticleNode : public ArgumentNode {
public:
        using ParamVector = QVector<NodePtr>;

        explicit ParticleNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        ParamVector params() const;

        template <typename ...Args>
        void setParams(Args&& ... params) {
            m_isValid = (params->isValid() && ...);
            (m_params << ... << std::forward<Args>(params));
        };

        QSharedPointer<ResourceLocationNode> resLoc() const;
        void setResLoc(QSharedPointer<ResourceLocationNode> newResLoc);

private:
        QSharedPointer<ResourceLocationNode> m_resLoc;
        ParamVector m_params;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Particle)
}

#endif /* PARTICLENODE_H */
