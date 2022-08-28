#include "nodecounter.h"

#include "../minecraftparser.h"

void Command::NodeCounterImpl::operator()(Command::ParseNode *node) {
}

void Command::NodeCounterImpl::operator()(Command::LiteralNode *node) {
    if (!node->isCommand())
        return;

    const QString &&command = node->text();

    if (commandCounts.contains(command))
        ++commandCounts[command];
    else
        commandCounts[command] = 1;
}

void Command::NodeCounterImpl::operator()(Command::TargetSelectorNode *node) {
    const auto varType = node->variable();

    if (targetSelectorCounts.contains(varType))
        ++targetSelectorCounts[varType];
    else
        targetSelectorCounts[varType] = 1;
}

void Command::NodeCounterImpl::operator()(
    Command::EntityArgumentValueNode *node) {
    if (dynamic_cast<Command::NbtCompoundNode*>(node->value().get()))
        ++nbtAccessCount;
}



void Command::NodeCounter::startVisiting(Command::ParseNode *node) {
    node->accept(this, NodeVisitor::Order::Preorder);
}

UintHash Command::NodeCounter::commandCounts() const {
    return impl.commandCounts;
}

SelectorHash Command::NodeCounter::targetSelectorCounts() const {
    return impl.targetSelectorCounts;
}

int Command::NodeCounter::nbtAccessCount() const {
    return impl.nbtAccessCount;
}
