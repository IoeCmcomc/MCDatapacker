#include "similarresourcelocationnodes.h"

#define DEFINE_TYPE_FROM_RESOURCELOCATIONNODE(Type, Id)                  \
    static const int _ ## Type = qRegisterMetaType<Command::Type*>();    \
    Command::Type::Type(QObject *parent, int pos, const QString &nspace, \
                        const QString &id)                               \
        : Command::ResourceLocationNode(parent, pos, nspace, id) {       \
        setParserId(Id);                                                 \
    }                                                                    \
    QString Command::Type::toString() const {                            \
        return ResourceLocationNode::toString().replace(0, 20, #Type);   \
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
