#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include "floatnode.h"
#include "blockstatenode.h"
#include "itemstacknode.h"
#include "resourcelocationnode.h"

namespace Command {
    class ParticleColorNode : public ParseNode {
public:
        ParticleColorNode(QObject *parent, int pos);
        QString toString() const override;
        bool isVaild() const override;

        FloatNode *r() const;
        void setR(FloatNode *r);

        FloatNode *g() const;
        void setG(FloatNode *g);

        FloatNode *b() const;
        void setB(FloatNode *b);

        FloatNode *size() const;
        void setSize(FloatNode *size);

private:
        FloatNode *m_r    = nullptr;
        FloatNode *m_g    = nullptr;
        FloatNode *m_b    = nullptr;
        FloatNode *m_size = nullptr;
    };

    class ParticleNode : public ResourceLocationNode
    {
        Q_OBJECT
public:
        ParticleNode(QObject *parent, int pos, const QString &nspace,
                     const QString &id);
        ParticleNode(ResourceLocationNode *other);
        QString toString() const override;

        ParseNode *params() const;
        void setParams(BlockStateNode *params);
        void setParams(ItemStackNode *params);
        void setParams(ParticleColorNode *params);
private:
        ParseNode *m_params = nullptr;
    };
}

Q_DECLARE_METATYPE(Command::ParticleColorNode*)
Q_DECLARE_METATYPE(Command::ParticleNode*)

#endif /* PARTICLENODE_H */
