#include "parsenodecache.h"

#include <QDebug>

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

    int ParseNodeCache::capacity() const {
        return m_cache.capacity();
    }

    void ParseNodeCache::setCapacity(const size_t capacity) {
        m_cache.capacity(capacity);
    }

    void ParseNodeCache::clear() {
        m_cache.clear();
    }

    bool ParseNodeCache::contains(const CacheKey &key) const {
        return m_cache.contains(key);
    }

    const WeakNodePtr &ParseNodeCache::lookup(
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }

    void ParseNodeCache::emplace(const int typeId, const QString &literalStr,
                                 WeakNodePtr node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr),
                        std::forward_as_tuple(node));
    }

    void ParseNodeCache::emplace(const int typeId, QString &&literalStr,
                                 WeakNodePtr node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr),
                        std::forward_as_tuple(node));
    }

    void ParseNodeCache::emplace(const int typeId, const QString &literalStr,
                                 const QVariantMap &props, WeakNodePtr node) {
        m_cache.emplace(std::piecewise_construct,
                        std::forward_as_tuple(typeId, literalStr, props),
                        std::forward_as_tuple(node));
    }

    WeakNodePtr &ParseNodeCache::operator [](const CacheKey &key) {
        return m_cache.lookup(key);
    }

    void ParseNodeCache::print() {
        for (const auto entry: m_cache) {
            qDebug() << entry.key().literalStr << entry.value();
        }
    }

    const WeakNodePtr &ParseNodeCache::operator[](
        const CacheKey &key) const {
        return m_cache.lookup(key);
    }
}
