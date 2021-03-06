#ifndef TARGETSELECTORNODE_H
#define TARGETSELECTORNODE_H

#include "parsenode.h"
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
            S
        };

        TargetSelectorNode(int pos);
        QString toString() const override;

        Variable variable() const;
        void setVariable(const Variable &variable);

        QSharedPointer<MapNode> args() const;
        void setArgs(QSharedPointer<MapNode> args);

private:
        Variable m_variable            = Variable::A;
        QSharedPointer<MapNode> m_args = nullptr;
        static const QMap<Variable, char> variableMap;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::TargetSelectorNode>)

#endif /* TARGETSELECTORNODE_H */
