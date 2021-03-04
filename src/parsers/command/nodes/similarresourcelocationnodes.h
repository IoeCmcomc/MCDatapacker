#ifndef SIMILARRESOURCELOCATIONNODES_H
#define SIMILARRESOURCELOCATIONNODES_H

#include "resourcelocationnode.h"

/*
   Template classes are not supported by Q_OBJECT, so I had to
   use this macro instead.
 */
#define DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(Type)                \
    class Type : public ResourceLocationNode {                      \
public:                                                             \
        explicit Type(int pos, const QString &nspace = "minecraft", \
                      const QString &id              = "");         \
        QString toString() const;                                   \
    };                                                              \
/* End of macro
 */

namespace Command {
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(DimensionNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(EntitySummonNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(FunctionNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(ItemEnchantmentNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(MobEffectNode)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::DimensionNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::EntitySummonNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::FunctionNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::ItemEnchantmentNode>);
Q_DECLARE_METATYPE(QSharedPointer<Command::MobEffectNode>)

#endif /* SIMILARRESOURCELOCATIONNODES_H */
