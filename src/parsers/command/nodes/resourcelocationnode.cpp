#include "resourcelocationnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    ResourceLocationNode::ResourceLocationNode(int length)
        : ArgumentNode(ParserType::ResourceLocation, length) {
    }

    ResourceLocationNode::ResourceLocationNode(int length,
                                               const SpanPtr &nspace,
                                               const SpanPtr &id)
        : ArgumentNode(ParserType::ResourceLocation, length),
        m_namespace(nspace), m_id(id) {
        m_isValid = nspace->isValid() && id->isValid();
    }

    ResourceLocationNode::ResourceLocationNode(ParserType parserType,
                                               int length,
                                               const SpanPtr &nspace,
                                               const SpanPtr &id)
        : ArgumentNode(parserType, length),
        m_namespace(nspace), m_id(id) {
    }

    ResourceLocationNode::ResourceLocationNode(ParserType parserType,
                                               int length)
        : ArgumentNode(parserType, length) {
    }

    DEFINE_ACCEPT_METHOD(ResourceLocationNode)

    SpanPtr ResourceLocationNode::nspace() const {
        return m_namespace;
    }

    void ResourceLocationNode::setNamespace(SpanPtr nspace) {
        m_namespace = std::move(nspace);
    }

    SpanPtr ResourceLocationNode::id() const {
        return m_id;
    }

    void ResourceLocationNode::setId(SpanPtr id) {
        m_isValid = m_namespace ? id->isValid() && m_namespace->isValid()
                                : id->isValid();
        m_id = std::move(id);
    }

    bool ResourceLocationNode::isTag() const {
        return m_isTag;
    }

    void ResourceLocationNode::setIsTag(bool isTag) {
        m_isTag = isTag;
    }

    DEFINE_ACCEPT_METHOD(DimensionNode)
    DEFINE_ACCEPT_METHOD(EntitySummonNode)
    DEFINE_ACCEPT_METHOD(FunctionNode)
    DEFINE_ACCEPT_METHOD(ItemEnchantmentNode)
    DEFINE_ACCEPT_METHOD(MobEffectNode)

    DEFINE_ACCEPT_METHOD(ResourceNode)
    DEFINE_ACCEPT_METHOD(ResourceKeyNode)
    DEFINE_ACCEPT_METHOD(ResourceOrTagNode)
    DEFINE_ACCEPT_METHOD(ResourceOrTagKeyNode)
}
