#ifndef SIMILARAXESNODES_H
#define SIMILARAXESNODES_H

#include "axesnode.h"

/*
   Template classes are not supported by Q_OBJECT, so I had to
   use this macro instead.
 */
#define DECLARE_TYPE_FROM_AXESNODE(Type)                      \
    class Type : public AxesNode {                            \
        Q_OBJECT                                              \
public:                                                       \
        explicit Type(QObject * parent, int pos, int length); \
        QString toString() const;                             \
    };

namespace Command {
    DECLARE_TYPE_FROM_AXESNODE(BlockPosNode)
    DECLARE_TYPE_FROM_AXESNODE(ColumnPosNode)
    DECLARE_TYPE_FROM_AXESNODE(RotationNode)
    DECLARE_TYPE_FROM_AXESNODE(Vec2Node)
    DECLARE_TYPE_FROM_AXESNODE(Vec3Node)
}

Q_DECLARE_METATYPE(Command::BlockPosNode*);
Q_DECLARE_METATYPE(Command::ColumnPosNode*);
Q_DECLARE_METATYPE(Command::RotationNode*);
Q_DECLARE_METATYPE(Command::Vec2Node*);
Q_DECLARE_METATYPE(Command::Vec3Node*);

#endif /* SIMILARAXESNODES_H */
