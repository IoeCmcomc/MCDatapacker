#include "floatrangenode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::FloatRangeNode> >();

Command::FloatRangeNode::FloatRangeNode(int pos, int length)
    : RangeNode(pos, length, "minecraft:float_range") {
}

QString Command::FloatRangeNode::toString() const {
    return QString("FloatRangeNode(%1)").arg(format());
}
