#ifndef PARSENODECACHE_H
#define PARSENODECACHE_H

#include "nodes/parsenode.h"
#include <lru-cache/include/lru/lru.hpp>

namespace Command {
    struct CacheKey {
        const int     typeId = 0;
        const QString literalStr;
        int           pos = -1;
        CacheKey(const int id, const QString &str, const int pos = -1) : typeId(
                id),
            literalStr(str), pos(pos) {
        }
        bool operator==(const CacheKey rhs) const;
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
        ParseNodeCache(int capacity = 4000);

        using NodePointerCache = LRU::Cache<CacheKey,
                                            QSharedPointer<ParseNode> >;

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
        void emplace(const int typeId, const QString &literalStr, int pos,
                     QSharedPointer<ParseNode> node);
        QSharedPointer<ParseNode> &operator[](const CacheKey &key);
        const QSharedPointer<ParseNode> &operator[](const CacheKey &key) const;

        const LRU::Statistics<CacheKey> &stats() const {
            return m_cache.stats();
        }

private:
        NodePointerCache m_cache;
    };
}

#endif /* PARSENODECACHE_H */
