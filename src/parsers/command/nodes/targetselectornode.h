#ifndef TARGETSELECTORNODE_H
#define TARGETSELECTORNODE_H

#include "parsenode.h"
#include "mapnode.h"

namespace Command {
    class TargetSelectorNode : public ParseNode
    {
        Q_OBJECT
public:
        enum class Variable {
            A,
            E,
            P,
            S
        };

        TargetSelectorNode(QObject *parent, int pos);
        QString toString() const override;

        Variable variable() const;
        void setVariable(const Variable &variable);

        MapNode *args() const;
        void setArgs(MapNode *args);

private:
        Variable m_variable = Variable::A;
        MapNode *m_args     = nullptr;
        static const QMap<Variable, char> variableMap;
    };
}

Q_DECLARE_METATYPE(Command::TargetSelectorNode*)

#endif /* TARGETSELECTORNODE_H */
