#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "floatnode.h"
#include "resourcelocationnode.h"

namespace Command {

    class ParticleColorNode : public ParseNode {
public:
        ParticleColorNode(int pos);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;

        QSharedPointer<FloatNode> r() const;
        void setR(QSharedPointer<FloatNode> r);

        QSharedPointer<FloatNode> g() const;
        void setG(QSharedPointer<FloatNode> g);

        QSharedPointer<FloatNode> b() const;
        void setB(QSharedPointer<FloatNode> b);

private:
        QSharedPointer<FloatNode> m_r    = nullptr;
        QSharedPointer<FloatNode> m_g    = nullptr;
        QSharedPointer<FloatNode> m_b    = nullptr;
    };

    class ParticleNode : public ResourceLocationNode
    {
public:
        using ParamVector = QVector<QSharedPointer<ParseNode>>;

        ParticleNode(int pos, const QString &nspace,
                     const QString &id);
        ParticleNode(ResourceLocationNode *other);
        QString toString() const override;
        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;

        ParamVector params() const;
        void setParams(std::initializer_list<QSharedPointer<ParseNode>> params);
private:
        ParamVector m_params;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ParticleColorNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ParticleNode>)

#endif /* PARTICLENODE_H */
