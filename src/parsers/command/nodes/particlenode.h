#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "argumentnode.h"

namespace Command {
    class FloatNode;
    class ResourceLocationNode;
    class NbtCompoundNode;

    class ParticleColorNode : public ParseNode {
public:
        explicit ParticleColorNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

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

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        ParamVector params() const;

        template <typename ...Args>
        void setParams(Args&& ... params) {
            m_isValid = (params->isValid() && ...);
            (m_params << ... << std::forward<Args>(params));
        };

        QSharedPointer<ResourceLocationNode> resLoc() const;
        void setResLoc(QSharedPointer<ResourceLocationNode> newResLoc);

        QSharedPointer<NbtCompoundNode> options() const;
        void setOptions(QSharedPointer<NbtCompoundNode> newOptions);

        bool hasParams() const;

private:
        QSharedPointer<ResourceLocationNode> m_resLoc;
        ParamVector m_params;                      // For pre-1.20.5 syntax
        QSharedPointer<NbtCompoundNode> m_options; // For 1.20.5+ syntax
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Particle)
}

#endif /* PARTICLENODE_H */
