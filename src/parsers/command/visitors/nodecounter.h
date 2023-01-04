#ifndef NODECOUNTER_H
#define NODECOUNTER_H

#include "overloadnodevisitor.h"
#include "parsers/command/nodes/targetselectornode.h"

#include <QString>
#include <QHash>

using UintHash     = QHash<QString, uint>;
using SelectorHash = QMap<Command::TargetSelectorNode::Variable, uint>;

namespace Command {
    struct NodeCounterImpl {
        void operator()(ParseNode *node);

        void operator()(LiteralNode *node);
        void operator()(TargetSelectorNode *node);
        void operator()(EntityArgumentValueNode *node);

        NodeVisitor *visitor        = nullptr;
        uint         nbtAccessCount = 0;
        UintHash     commandCounts;
        SelectorHash targetSelectorCounts;
    };


    class NodeCounter : public OverloadNodeVisitor<NodeCounterImpl> {
public:
        NodeCounter() : OverloadNodeVisitor(Postorder) {
        };

        void startVisiting(ParseNode *node) override;
        UintHash commandCounts() const;
        SelectorHash targetSelectorCounts() const;
        int nbtAccessCount() const;
    };
}

#endif /* NODECOUNTER_H */
