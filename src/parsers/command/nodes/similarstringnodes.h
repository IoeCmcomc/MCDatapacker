#ifndef SIMILARSTRINGNODES_H
#define SIMILARSTRINGNODES_H

#include "stringnode.h"

/*
   Template classes hasn't supported literal strings as arguments,
   so I had to use this macro instead.
 */
#define DECLARE_TYPE_FROM_STRINGNODE(Type)          \
    class Type : public StringNode {                \
public:                                             \
        explicit Type(int pos, const QString &txt); \
        QString toString() const;                   \
    };                                              \
/* End of macro
 */

namespace Command {
    DECLARE_TYPE_FROM_STRINGNODE(ColorNode)
    DECLARE_TYPE_FROM_STRINGNODE(EntityAnchorNode)
    DECLARE_TYPE_FROM_STRINGNODE(ItemSlotNode)
    DECLARE_TYPE_FROM_STRINGNODE(MessageNode)
    DECLARE_TYPE_FROM_STRINGNODE(ObjectiveNode)
    DECLARE_TYPE_FROM_STRINGNODE(ObjectiveCriteriaNode)
    DECLARE_TYPE_FROM_STRINGNODE(OperationNode)
    DECLARE_TYPE_FROM_STRINGNODE(ScoreboardSlotNode)
    DECLARE_TYPE_FROM_STRINGNODE(TeamNode)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ColorNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::EntityAnchorNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ItemSlotNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::MessageNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ObjectiveNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ObjectiveCriteriaNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::OperationNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ScoreboardSlotNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::TeamNode>);

#endif /* SIMILARSTRINGNODES_H */
