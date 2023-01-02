#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "singlevaluenode.h"
#include "resourcelocationnode.h"

namespace Command {
    class ParticleColorNode : public ParseNode {
public:
        explicit ParticleColorNode(int length);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        QSharedPointer<FloatNode> r() const;
        void setR(QSharedPointer<FloatNode> r);

        QSharedPointer<FloatNode> g() const;
        void setG(QSharedPointer<FloatNode> g);

        QSharedPointer<FloatNode> b() const;
        void setB(QSharedPointer<FloatNode> b);

private:
        QSharedPointer<FloatNode> m_r = nullptr;
        QSharedPointer<FloatNode> m_g = nullptr;
        QSharedPointer<FloatNode> m_b = nullptr;
    };

    class ParticleNode : public ResourceLocationNode
    {
public:
        using ParamVector = QVector<NodePtr>;

        explicit ParticleNode(int length);
        explicit ParticleNode(ResourceLocationNode *other);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        ParamVector params() const;
        void setParams(
            std::initializer_list<NodePtr> params);
private:
        ParamVector m_params;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Particle)
}

#endif /* PARTICLENODE_H */
