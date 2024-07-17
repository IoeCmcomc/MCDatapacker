#include "nodecounter.h"

//#include "../minecraftparser.h"

namespace Command {
    void NodeCounter::visit(LiteralNode *node) {
        if (!node->isCommand())
            return;

        const QString &&command = node->text();

        if (m_commandCounts.contains(command))
            ++m_commandCounts[command];
        else
            m_commandCounts[command] = 1;
    }

    void NodeCounter::visit(TargetSelectorNode *node) {
        const auto varType = node->variable();

        if (m_targetSelectorCounts.contains(varType))
            ++m_targetSelectorCounts[varType];
        else
            m_targetSelectorCounts[varType] = 1;
    }

    void NodeCounter::visit(
        InvertibleNode *node) {
        if (const auto *parent = node->getNode().get()) {
            if (parent->kind() == ParseNode::Kind::Argument) {
                if (((ArgumentNode *)parent)->parserType() ==
                    ArgumentNode::ParserType::NbtCompoundTag) {
                    ++m_nbtAccessCount;
                }
            }
        }
    }

    UintHash NodeCounter::commandCounts() const {
        return m_commandCounts;
    }

    SelectorHash NodeCounter::targetSelectorCounts() const {
        return m_targetSelectorCounts;
    }

    int NodeCounter::nbtAccessCount() const {
        return m_nbtAccessCount;
    }
}
