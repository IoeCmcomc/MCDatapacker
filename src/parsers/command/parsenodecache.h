#ifndef PARSENODECACHE_H
#define PARSENODECACHE_H

#include "nodes/parsenode.h"
#include "lru/lru.hpp"

#include <QBuffer>
#include <QDataStream>

namespace Command {
    struct CacheKey {
        const int         typeId = 0;
        const QString     literalStr;
        const QVariantMap props;
        CacheKey(const int id, const QString &str,
                 const QVariantMap &p = QVariantMap()) : typeId(id),
            literalStr(str), props(p) {
        }
        bool operator==(const CacheKey &rhs) const;
    };
}

// Adapted from: https://stackoverflow.com/a/41145546/12682038
class QVariantMapHasher {
public:
    QVariantMapHasher() : buff(&bb), ds(&buff) {
        bb.reserve(1000);
        buff.open(QIODevice::WriteOnly);
    }
    uint hash(const QVariantMap & v) {
        buff.seek(0);
        ds << v;
        return qHashBits(bb.constData(), buff.pos());
    }

private:
    QByteArray bb;
    QBuffer buff;
    QDataStream ds;
};

namespace std {
    template<> struct hash<Command::CacheKey> {
        using argument_type = Command::CacheKey;
        using result_type   = std::size_t;

        result_type operator()(argument_type const& key) const {
            static QVariantMapHasher variantMapHash;

            result_type const h1(std::hash<int>()(key.typeId));
            result_type const h2(std::hash<QString>()(key.literalStr));
            result_type const h3(variantMapHash.hash(key.props));

            return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
        }
    };
}

namespace Command {
    using WeakNodePtr = QWeakPointer<ParseNode>;

    class ParseNodeCache
    {
public:
        explicit ParseNodeCache(int capacity = 64);

        using Cache = LRU::Cache<CacheKey, WeakNodePtr>;

        bool isEmpty() const;
        int size() const;
        int capacity() const;
        void setCapacity(const size_t capacity);
        void clear();
        bool contains(const CacheKey &key) const;
        const WeakNodePtr &lookup(const CacheKey &key) const;
        void emplace(const int typeId, const QString &literalStr,
                     WeakNodePtr node);
        void emplace(const int typeId, QString &&literalStr, WeakNodePtr node);
        void emplace(const int typeId, const QString &literalStr,
                     const QVariantMap &props, WeakNodePtr node);
        WeakNodePtr &operator[](const CacheKey &key);
        const WeakNodePtr &operator[](const CacheKey &key) const;

        const LRU::Statistics<CacheKey> &stats() const {
            return m_cache.stats();
        }

        void print() {
            for (const auto entry: m_cache) {
                qDebug() << entry.key().literalStr << entry.value();
            }
        }

private:
        Cache m_cache;
    };
}

#endif /* PARSENODECACHE_H */
