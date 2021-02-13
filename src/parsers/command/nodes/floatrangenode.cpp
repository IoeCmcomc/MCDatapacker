#include "floatrangenode.h"

static int _ = qRegisterMetaType<Command::FloatRangeNode*>();

Command::FloatRangeNode::FloatRangeNode(QObject *parent, int pos, int length)
    : Command::ArgumentNode(parent, pos, length, "minecraft:float_range") {
    setExactValue(new Command::FloatNode(this));
}

QString Command::FloatRangeNode::toString() const {
    return QString("FloatRangeNode(%1)").arg(format());
}
