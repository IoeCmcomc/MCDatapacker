#include "intrangenode.h"

static int _ = qRegisterMetaType<Command::IntRangeNode*>();

Command::IntRangeNode::IntRangeNode(QObject *parent, int pos, int length)
    : Command::ArgumentNode(parent, pos, length, "minecraft:int_range") {
    setExactValue(new Command::IntegerNode(this));
}

QString Command::IntRangeNode::toString() const {
    return QString("IntRangeNode(%1)").arg(format());
}
