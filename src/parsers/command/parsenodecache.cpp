#include "parsenodecache.h"

namespace Command {
    bool CacheKey::operator==(const CacheKey &rhs) const {
        return (typeId == rhs.typeId) && (literalStr == rhs.literalStr);
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

    const QSharedPointer<ParseNode> &ParseNodeCache::lookup(
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }

    void ParseNodeCache::emplace(const int typeId, const QString &literalStr,
                                 QSharedPointer<ParseNode> node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr),
                        std::forward_as_tuple(node));
    }

    NodePtr &ParseNodeCache::operator [](const CacheKey &key) {
        return m_cache.lookup(key);
    }

    const QSharedPointer<ParseNode> &ParseNodeCache::operator[](
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }
}
