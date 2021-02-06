#ifndef SIMILARRESOURCELOCATIONNODES_H
#define SIMILARRESOURCELOCATIONNODES_H

#include "resourcelocationnode.h"

/*
   Template classes are not supported by Q_OBJECT, so I had to
   use this macro instead.
 */
#define DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(Type)       \
    class Type : public ResourceLocationNode {             \
        Q_OBJECT                                           \
public:                                                    \
        explicit Type(QObject * parent, int pos,           \
                      const QString &nspace = "minecraft", \
                      const QString &id     = "");         \
        QString toString() const;                          \
    };                                                     \
/* End of macro
 */

namespace Command {
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(DimensionNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(EntitySummonNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(FunctionNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(ItemEnchantmentNode)
    DECLARE_TYPE_FROM_RESOURCELOCATIONNODE(MobEffectNode)
}

Q_DECLARE_METATYPE(Command::DimensionNode*);
Q_DECLARE_METATYPE(Command::EntitySummonNode*);
Q_DECLARE_METATYPE(Command::FunctionNode*);
Q_DECLARE_METATYPE(Command::ItemEnchantmentNode*);
Q_DECLARE_METATYPE(Command::MobEffectNode*)

#endif /* SIMILARRESOURCELOCATIONNODES_H */
