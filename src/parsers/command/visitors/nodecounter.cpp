#include "nodecounter.h"

//#include "../minecraftparser.h"

namespace Command {
    void NodeCounterImpl::operator()(ParseNode *node) {
    }

    void NodeCounterImpl::operator()(LiteralNode *node) {
        if (!node->isCommand())
            return;

        const QString &&command = node->text();

        if (commandCounts.contains(command))
            ++commandCounts[command];
        else
            commandCounts[command] = 1;
    }

    void NodeCounterImpl::operator()(TargetSelectorNode *node) {
        const auto varType = node->variable();

        if (targetSelectorCounts.contains(varType))
            ++targetSelectorCounts[varType];
        else
            targetSelectorCounts[varType] = 1;
    }

    void NodeCounterImpl::operator()(
        EntityArgumentValueNode *node) {
        if (node->getNode() && (node->getNode()->parserType() ==
                                ArgumentNode::ParserType::NbtCompoundTag)) {
            ++nbtAccessCount;
        }
    }



    void NodeCounter::startVisiting(ParseNode *node) {
        node->accept(this, VisitOrder::Preorder);
    }

    UintHash NodeCounter::commandCounts() const {
        return impl.commandCounts;
    }

    SelectorHash NodeCounter::targetSelectorCounts() const {
        return impl.targetSelectorCounts;
    }

    int NodeCounter::nbtAccessCount() const {
        return impl.nbtAccessCount;
    }
}
