#include "intrangenode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::IntRangeNode> >();

Command::IntRangeNode::IntRangeNode(int pos, int length)
    : RangeNode(pos, length, "minecraft:int_range") {
}

QString Command::IntRangeNode::toString() const {
    return QString("IntRangeNode(%1)").arg(format());
}
