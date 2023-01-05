#ifndef NODECOUNTER_H
#define NODECOUNTER_H

#include "overloadnodevisitor.h"
#include "parsers/command/nodes/targetselectornode.h"

#include <QString>
#include <QHash>

using UintHash     = QHash<QString, uint>;
using SelectorHash = QMap<Command::TargetSelectorNode::Variable, uint>;

namespace Command {
    class NodeCounter : public OverloadNodeVisitor {
public:
        NodeCounter() : OverloadNodeVisitor(Postorder) {
        };

        void visit(LiteralNode *node) override;
        void visit(TargetSelectorNode *node) override;
        void visit(EntityArgumentValueNode *node) override;

        UintHash commandCounts() const;
        SelectorHash targetSelectorCounts() const;
        int nbtAccessCount() const;

private:
        uint m_nbtAccessCount = 0;
        UintHash m_commandCounts;
        SelectorHash m_targetSelectorCounts;
    };
}

#endif /* NODECOUNTER_H */
