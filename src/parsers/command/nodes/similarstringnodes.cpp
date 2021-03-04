#include "similarstringnodes.h"

#define DEFINE_TYPE_FROM_STRINGNODE(Type, Id)                                 \
    static const int _ ## Type =                                              \
        qRegisterMetaType<QSharedPointer<Command::Type> >();                  \
    Command::Type::Type(int pos, const QString &txt) : StringNode(pos, txt) { \
        setParserId(Id);                                                      \
    }                                                                         \
    QString Command::Type::toString() const {                                 \
        return #Type + QString("(\"%1\")").arg(value());                      \
    }

DEFINE_TYPE_FROM_STRINGNODE(ColorNode, "minecraft:color")
DEFINE_TYPE_FROM_STRINGNODE(EntityAnchorNode, "minecraft:entity_anchor")
DEFINE_TYPE_FROM_STRINGNODE(ItemSlotNode, "minecraft:item_slot")
DEFINE_TYPE_FROM_STRINGNODE(MessageNode, "minecraft:message")
DEFINE_TYPE_FROM_STRINGNODE(ObjectiveNode, "minecraft:objective")
DEFINE_TYPE_FROM_STRINGNODE(ObjectiveCriteriaNode,
                            "minecraft:objective_criteria")
DEFINE_TYPE_FROM_STRINGNODE(OperationNode, "minecraft:operation")
DEFINE_TYPE_FROM_STRINGNODE(ScoreboardSlotNode, "minecraft:scoreboard_slot")
DEFINE_TYPE_FROM_STRINGNODE(TeamNode, "minecraft:team")
