#include "similarstringnodes.h"

#define DEFINE_TYPE_FROM_STRINGNODE(Type, Id)                         \
    Command::Type::Type(QObject *parent, int pos,                     \
                        const QString &txt) : StringNode(parent, pos, \
                                                         txt) {       \
        qRegisterMetaType<Command::Type*>();                          \
        setParserId(Id);                                              \
    }                                                                 \
                                                                      \
    QString Command::Type::toString() const {                         \
        return #Type + QString("(\"%1\")").arg(value());              \
    }

DEFINE_TYPE_FROM_STRINGNODE(ColorNode, "minecraft:color")
DEFINE_TYPE_FROM_STRINGNODE(EntityAnchorNode, "minecraft:entity_anchor")
DEFINE_TYPE_FROM_STRINGNODE(OperationNode, "minecraft:operation")
