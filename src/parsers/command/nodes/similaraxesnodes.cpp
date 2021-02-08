#include "similaraxesnodes.h"

#define DEFINE_TYPE_FROM_AXESNODE(Type, Id)                           \
    static const int _ ## Type = qRegisterMetaType<Command::Type*>(); \
    Command::Type::Type(QObject *parent, int pos, int length)         \
        : Command::AxesNode(parent, pos, length) {                    \
        setParserId(Id);                                              \
    }                                                                 \
    QString Command::Type::toString() const {                         \
        return AxesNode::toString().replace(0, 8, #Type);             \
    }

DEFINE_TYPE_FROM_AXESNODE(BlockPosNode, "minecraft:block_pos")
DEFINE_TYPE_FROM_AXESNODE(ColumnPosNode, "minecraft:column_pos")
DEFINE_TYPE_FROM_AXESNODE(RotationNode, "minecraft:rotation")
DEFINE_TYPE_FROM_AXESNODE(Vec2Node, "minecraft:vec2")
DEFINE_TYPE_FROM_AXESNODE(Vec3Node, "minecraft:vec3")
