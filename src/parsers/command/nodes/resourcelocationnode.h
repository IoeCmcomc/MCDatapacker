#ifndef RESOURCELOCATIONNODE_H
#define RESOURCELOCATIONNODE_H

#include "argumentnode.h"

namespace Command {
    class ResourceLocationNode : public ArgumentNode
    {
public:
        explicit ResourceLocationNode(int length);
        explicit ResourceLocationNode(int length,
                                      const SpanPtr &nspace,
                                      const SpanPtr &id);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        SpanPtr nspace() const;
        void setNamespace(SpanPtr nspace);

        SpanPtr id() const;
        void setId(SpanPtr id);

        bool isTag() const;
        void setIsTag(bool isTag);

protected:
        SpanPtr m_namespace = nullptr;
        SpanPtr m_id        = nullptr;
        bool m_isTag        = false;

        explicit ResourceLocationNode(ParserType parserType, int length,
                                      const SpanPtr &nspace, const SpanPtr &id);
        explicit ResourceLocationNode(ParserType parserType, int length);
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ResourceLocation)
}


#define DECLARE_RESOURCE_LOCATION_NODE_CLASS(Name)                       \
        namespace Command {                                              \
            class Name ## Node : public ResourceLocationNode  {          \
public:                                                                  \
                explicit Name ## Node(int length)                        \
                    : ResourceLocationNode(ParserType::Name, length) {   \
                };                                                       \
                void accept(NodeVisitor * visitor, VisitOrder) override; \
            };                                                           \
        }                                                                \

DECLARE_RESOURCE_LOCATION_NODE_CLASS(Dimension)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(EntitySummon)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(Function)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(ItemEnchantment)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(MobEffect)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(Resource)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(ResourceKey)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(ResourceOrTag)
DECLARE_RESOURCE_LOCATION_NODE_CLASS(ResourceOrTagKey)

#undef DECLARE_RESOURCE_LOCATION_NODE_CLASS

#endif /* RESOURCELOCATIONNODE_H */
