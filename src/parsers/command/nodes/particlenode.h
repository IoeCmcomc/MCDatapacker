#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "floatnode.h"
#include "blockstatenode.h"
#include "itemstacknode.h"
#include "resourcelocationnode.h"

namespace Command {
    class ParticleColorNode : public ParseNode {
public:
        ParticleColorNode(int pos);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            m_r->accept(visitor);
            m_g->accept(visitor);
            m_b->accept(visitor);
            m_size->accept(visitor);
            visitor->visit(this);
        }

        QSharedPointer<FloatNode> r() const;
        void setR(QSharedPointer<FloatNode> r);

        QSharedPointer<FloatNode> g() const;
        void setG(QSharedPointer<FloatNode> g);

        QSharedPointer<FloatNode> b() const;
        void setB(QSharedPointer<FloatNode> b);

        QSharedPointer<FloatNode> size() const;
        void setSize(QSharedPointer<FloatNode> size);

private:
        QSharedPointer<FloatNode> m_r    = nullptr;
        QSharedPointer<FloatNode> m_g    = nullptr;
        QSharedPointer<FloatNode> m_b    = nullptr;
        QSharedPointer<FloatNode> m_size = nullptr;
    };

    class ParticleNode : public ResourceLocationNode
    {
public:
        ParticleNode(int pos, const QString &nspace,
                     const QString &id);
        ParticleNode(ResourceLocationNode *other);
        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            if (m_params)
                m_params->accept(visitor);
            visitor->visit(this);
        }

        QSharedPointer<ParseNode> params() const;
        void setParams(QSharedPointer<BlockStateNode> params);
        void setParams(QSharedPointer<ItemStackNode> params);
        void setParams(QSharedPointer<ParticleColorNode> params);
private:
        QSharedPointer<ParseNode> m_params = nullptr;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ParticleColorNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ParticleNode>)

#endif /* PARTICLENODE_H */
