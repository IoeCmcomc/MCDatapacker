#ifndef PARSENODECACHE_H
#define PARSENODECACHE_H

#include "nodes/parsenode.h"
#include "lru/lru.hpp"

namespace Command {
    struct CacheKey {
        const int     typeId = 0;
        const QString literalStr;
        CacheKey(const int id, const QString &str) : typeId(id),
            literalStr(str) {
        }
        bool operator==(const CacheKey &rhs) const;
    };
}

namespace std {
    template<> struct hash<Command::CacheKey> {
        using argument_type = Command::CacheKey;
        using result_type   = std::size_t;
        result_type operator()(argument_type const& key) const {
            result_type const h1(std::hash<int>()(key.typeId));
            result_type const h2(std::hash<QString>()(key.literalStr));

            return h1 ^ (h2 << 1);
        }
    };
}

namespace Command {
    class ParseNodeCache
    {
public:
        ParseNodeCache(int capacity = 500);

        using Cache = LRU::Cache<CacheKey, NodePtr>;

        bool isEmpty() const;
        int size() const;
        int capacity() const {
            return m_cache.capacity();
        }
        void clear();
        bool contains(const CacheKey &key) const;
        const QSharedPointer<ParseNode> &lookup(const CacheKey &key) const;
        void emplace(const int typeId, const QString &literalStr,
                     QSharedPointer<ParseNode> node);
        NodePtr &operator[](const CacheKey &key);
        const NodePtr &operator[](const CacheKey &key) const;

        const LRU::Statistics<CacheKey> &stats() const {
            return m_cache.stats();
        }

private:
        Cache m_cache;
    };
}

#endif /* PARSENODECACHE_H */
