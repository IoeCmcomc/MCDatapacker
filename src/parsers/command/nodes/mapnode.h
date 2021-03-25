#ifndef MAPNODE_H
#define MAPNODE_H

#include "literalnode.h"

namespace Command {
/*
   I found the idea of making MapNode inherit both ParseNode and QMap
   when searching for a way to implement SnbtCompound as a subclass
   of SnbtNode and MapNode. They inherits QObject, which makes
   it nearly impossible to inherit both of them.
   If MapNode inherits ParseNode and QMap, MapNode will have
   functionalities of both of them. Then, SnbtCompound can just inherit
   MapNode.

    A few days later, I searched about multiple inheritence
   and it is said that "Prefer composition over inheritance".
   Therefore, I changed my mind.
 */
    struct MapKey {
        int     pos = -1;
        QString text;
        bool    isQuote   = false;
        bool    sortByPos = true;
        bool operator<(const MapKey &other) const;
    };
    using ParseNodeMap = QMap<MapKey, QSharedPointer<ParseNode> >;

    class MapNode : public ParseNode
    {
public:
        MapNode(int pos, int length = 0);

        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            for (auto i = m_map.cbegin(); i != m_map.cend(); ++i) {
                visitor->visit(i.key());
                i.value()->accept(visitor);
            }
            visitor->visit(this);
        }

        int size() const;
        bool contains(const QString &key) const;
        bool contains(const MapKey &key) const;
        ParseNodeMap::const_iterator find(const QString &key) const;
        void insert(const MapKey &key, QSharedPointer<ParseNode> node);
        int remove(const MapKey &key);
        void clear();
        QSharedPointer<ParseNode> &operator[](const MapKey &key);
        const QSharedPointer<ParseNode> operator[](const MapKey &key) const;
        ParseNodeMap toMap() const;

private:
        ParseNodeMap m_map;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::MapNode>)

#endif /* MAPNODE_H */
