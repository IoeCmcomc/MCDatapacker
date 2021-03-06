#include "parsenodecache.h"

using NodePointerCache = LRU::Cache<Command::CacheKey,
                                    QSharedPointer<Command::ParseNode> >;

Command::ParseNodeCache::ParseNodeCache(int capacity)
    : m_cache(capacity) {
    m_cache.monitor();
}

bool Command::ParseNodeCache::isEmpty() const {
    return m_cache.is_empty();
}

int Command::ParseNodeCache::size() const {
    return m_cache.size();
}

void Command::ParseNodeCache::clear() {
    m_cache.clear();
}

bool Command::ParseNodeCache::contains(const CacheKey &key) const {
    return m_cache.contains(key);
}

const QSharedPointer<Command::ParseNode> &Command::ParseNodeCache::lookup(
    const Command::CacheKey &key) const {
    return m_cache.lookup(key);
}

void Command::ParseNodeCache::emplace(const int typeId,
                                      const QString &literalStr,
                                      QSharedPointer<ParseNode> node) {
    m_cache.emplace(std::piecewise_construct,
                    std::forward_as_tuple(typeId, literalStr),
                    std::forward_as_tuple(node));
}

QSharedPointer<Command::ParseNode> &Command::ParseNodeCache::operator[](
    const Command::CacheKey &key) {
    return m_cache.lookup(key);
}

const QSharedPointer<Command::ParseNode> &Command::ParseNodeCache::operator[](
    const Command::CacheKey &key) const {
    return m_cache.lookup(key);
}
