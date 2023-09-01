#include "singlevaluenode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    void FloatNode::chopTrailingDot() {
        if (text().back() == '.') {
            setText(text().chopped(1));
        }
    }

    DEFINE_ACCEPT_METHOD(BoolNode)
    DEFINE_ACCEPT_METHOD(DoubleNode)
    DEFINE_ACCEPT_METHOD(FloatNode)
    DEFINE_ACCEPT_METHOD(IntegerNode)
    DEFINE_ACCEPT_METHOD(LongNode)

    DEFINE_ACCEPT_METHOD(ColorNode)
    DEFINE_ACCEPT_METHOD(EntityAnchorNode)
    DEFINE_ACCEPT_METHOD(HeightmapNode)
    DEFINE_ACCEPT_METHOD(ItemSlotNode)
    DEFINE_ACCEPT_METHOD(MessageNode)
    DEFINE_ACCEPT_METHOD(ObjectiveNode)
    DEFINE_ACCEPT_METHOD(ObjectiveCriteriaNode)
    DEFINE_ACCEPT_METHOD(OperationNode)
    DEFINE_ACCEPT_METHOD(ScoreboardSlotNode)
    DEFINE_ACCEPT_METHOD(TeamNode)
    DEFINE_ACCEPT_METHOD(TemplateMirrorNode)
    DEFINE_ACCEPT_METHOD(TemplateRotationNode)

    DEFINE_ACCEPT_METHOD(UuidNode)

    DEFINE_ACCEPT_METHOD(InternalGreedyStringNode)
    DEFINE_ACCEPT_METHOD(InternalRegexPatternNode)
}
