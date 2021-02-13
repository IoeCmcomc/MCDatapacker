#ifndef BLOCKSTATENODE_H
#define BLOCKSTATENODE_H

#include "mapnode.h"
#include "resourcelocationnode.h"

namespace Command {
    class BlockStateNode : public ResourceLocationNode
    {
        Q_OBJECT
public:
        BlockStateNode(QObject *parent, int pos, const QString &nspace,
                       const QString &id);
        QString toString() const override;
        bool isVaild() const override;

        MapNode *states() const;
        void setStates(MapNode *states);

        MapNode *nbt() const;
        void setNbt(MapNode *nbt);

private:
        MapNode *m_states = nullptr;
        MapNode *m_nbt    = nullptr;
    };
}

#endif /* BLOCKSTATENODE_H */
