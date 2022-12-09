#include "floatrangenode.h"

const static bool _ = TypeRegister<Command::FloatRangeNode>::init();

Command::FloatRangeNode::FloatRangeNode(int pos, int length)
    : RangeNode(pos, length, "minecraft:float_range") {
}

QString Command::FloatRangeNode::toString() const {
    return QString("FloatRangeNode(%1)").arg(format());
}
