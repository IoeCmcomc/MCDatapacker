#include "similaraxesnodes.h"

#define DEFINE_TYPE_FROM_AXESNODE(Type, Id)                                \
        const static bool _ ## Type = TypeRegister<Command::Type>::init(); \
        Command::Type::Type(int pos, int length)                           \
            : Command::AxesNode(pos, length) {                             \
            setParserId(Id);                                               \
        }                                                                  \
        QString Command::Type::toString() const {                          \
            return AxesNode::toString().replace(0, 8, #Type);              \
        }

DEFINE_TYPE_FROM_AXESNODE(BlockPosNode, "minecraft:block_pos")
DEFINE_TYPE_FROM_AXESNODE(ColumnPosNode, "minecraft:column_pos")
DEFINE_TYPE_FROM_AXESNODE(RotationNode, "minecraft:rotation")
DEFINE_TYPE_FROM_AXESNODE(Vec2Node, "minecraft:vec2")
DEFINE_TYPE_FROM_AXESNODE(Vec3Node, "minecraft:vec3")
