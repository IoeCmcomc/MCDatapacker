#include "similarresourcelocationnodes.h"

#define DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(Type, Id)                  \
    Command::Type::Type(QObject *parent, int pos, const QString &nspace, \
                        const QString &id)                               \
        : Command::ResourceLocationNode(parent, pos, nspace, id) {       \
        qMetaTypeId<Command::Type*>();                                   \
        setParserId(Id);                                                 \
    }                                                                    \
    QString Command::Type::toString() const {                            \
        return #Type + QString("(%1:%2)").arg(nspace(), id());           \
    }

DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(DimensionNode, "minecraft:dimension")
DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(EntitySummonNode,
                                      "minecraft:entity_summon")
DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(FunctionNode,
                                      "minecraft:function")
DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(ItemEnchantmentNode,
                                      "minecraft:item_enchantment")
DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(MobEffectNode,
                                      "minecraft:mob_effect")
