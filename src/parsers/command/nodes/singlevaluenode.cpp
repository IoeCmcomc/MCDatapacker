#include "singlevaluenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    DEFINE_ACCEPT_METHOD(BoolNode)
    DEFINE_ACCEPT_METHOD(DoubleNode)
    DEFINE_ACCEPT_METHOD(FloatNode)
    DEFINE_ACCEPT_METHOD(IntegerNode)

    DEFINE_ACCEPT_METHOD(ColorNode)
    DEFINE_ACCEPT_METHOD(EntityAnchorNode)
    DEFINE_ACCEPT_METHOD(ItemSlotNode)
    DEFINE_ACCEPT_METHOD(MessageNode)
    DEFINE_ACCEPT_METHOD(ObjectiveNode)
    DEFINE_ACCEPT_METHOD(ObjectiveCriteriaNode)
    DEFINE_ACCEPT_METHOD(OperationNode)
    DEFINE_ACCEPT_METHOD(ScoreboardSlotNode)
    DEFINE_ACCEPT_METHOD(TeamNode)

    DEFINE_ACCEPT_METHOD(UuidNode)
}
