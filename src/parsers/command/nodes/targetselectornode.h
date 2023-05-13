#ifndef TARGETSELECTORNODE_H
#define TARGETSELECTORNODE_H

#include "mapnode.h"

namespace Command {
    class TargetSelectorNode : public ParseNode
    {
public:
        enum class Variable {
            Unknown = -1,
            A,
            E,
            P,
            R,
            S,
        };

        explicit TargetSelectorNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        Variable variable() const;
        void setVariable(const Variable &variable);

        QSharedPointer<MapNode> args() const;
        void setArgs(QSharedPointer<MapNode> args);

private:
        Variable m_variable            = Variable::Unknown;
        QSharedPointer<MapNode> m_args = nullptr;
    };
}

#endif /* TARGETSELECTORNODE_H */
