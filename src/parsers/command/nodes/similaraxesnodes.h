#ifndef SIMILARAXESNODES_H
#define SIMILARAXESNODES_H

#include "axesnode.h"


#define DECLARE_TYPE_FROM_AXESNODE(Type)    \
    class Type : public AxesNode {          \
public:                                     \
        explicit Type(int pos, int length); \
        QString toString() const;           \
    };

namespace Command {
    DECLARE_TYPE_FROM_AXESNODE(BlockPosNode)
    DECLARE_TYPE_FROM_AXESNODE(ColumnPosNode)
    DECLARE_TYPE_FROM_AXESNODE(RotationNode)
    DECLARE_TYPE_FROM_AXESNODE(Vec2Node)
    DECLARE_TYPE_FROM_AXESNODE(Vec3Node)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::BlockPosNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ColumnPosNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::RotationNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::Vec2Node>);
Q_DECLARE_METATYPE(QSharedPointer<Command::Vec3Node>);

#endif /* SIMILARAXESNODES_H */
