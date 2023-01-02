#include "parsenodecache.h"

namespace Command {
    bool CacheKey::operator==(const CacheKey &rhs) const {
        return (typeId == rhs.typeId) && (literalStr == rhs.literalStr)
               && (props == rhs.props);
    }

    ParseNodeCache::ParseNodeCache(int capacity)  : m_cache(capacity) {
        m_cache.monitor();
    }

    bool ParseNodeCache::isEmpty() const {
        return m_cache.is_empty();
    }

    int ParseNodeCache::size() const {
        return m_cache.size();
    }

    void ParseNodeCache::clear() {
        m_cache.clear();
    }

    bool ParseNodeCache::contains(const CacheKey &key) const {
        return m_cache.contains(key);
    }

    const NodePtr &ParseNodeCache::lookup(
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }

    void ParseNodeCache::emplace(const int typeId, const QString &literalStr,
                                 NodePtr node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr),
                        std::forward_as_tuple(node));
    }

    void ParseNodeCache::emplace(const int typeId, const QString &literalStr,
                                 const QVariantMap &props, NodePtr node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr, props),
                        std::forward_as_tuple(node));
    }

    NodePtr &ParseNodeCache::operator [](const CacheKey &key) {
        return m_cache.lookup(key);
    }

    const NodePtr &ParseNodeCache::operator[](
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }
}
