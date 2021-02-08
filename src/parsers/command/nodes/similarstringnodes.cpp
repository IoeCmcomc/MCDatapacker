#include "similarstringnodes.h"

#define DEFINE_TYPE_FROM_STRINGNODE(Type, Id)                         \
    static const int _ ## Type = qRegisterMetaType<Command::Type*>(); \
    Command::Type::Type(QObject *parent, int pos, const QString &txt) \
        : StringNode(parent, pos, txt) {                              \
        setParserId(Id);                                              \
    }                                                                 \
    QString Command::Type::toString() const {                         \
        return #Type + QString("(\"%1\")").arg(value());              \
    }

DEFINE_TYPE_FROM_STRINGNODE(ColorNode, "minecraft:color")
DEFINE_TYPE_FROM_STRINGNODE(EntityAnchorNode, "minecraft:entity_anchor")
DEFINE_TYPE_FROM_STRINGNODE(OperationNode, "minecraft:operation")