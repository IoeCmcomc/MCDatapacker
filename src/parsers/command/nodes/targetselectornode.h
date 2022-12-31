#ifndef TARGETSELECTORNODE_H
#define TARGETSELECTORNODE_H

#include "mapnode.h"

namespace Command {
    class TargetSelectorNode : public ParseNode
    {
public:
        enum class Variable {
            A,
            E,
            P,
            R,
            S,
        };

        explicit TargetSelectorNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        Variable variable() const;
        void setVariable(const Variable &variable);

        QSharedPointer<MapNode> args() const;
        void setArgs(QSharedPointer<MapNode> args);

private:
        Variable m_variable            = Variable::A;
        QSharedPointer<MapNode> m_args = nullptr;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::TargetSelectorNode>)

#endif /* TARGETSELECTORNODE_H */
