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
        bool operator<(const MapKey &other) const;
    };
    using ParseNodeMap = QMap<MapKey, ParseNode*>;

    class MapNode : public ParseNode
    {
        Q_OBJECT
public:
        MapNode(QObject *parent, int pos, int length = 0);

        QString toString() const override;

        int size() const;
        bool contains(const QString &key) const;
        bool contains(const MapKey &key) const;
        ParseNodeMap::const_iterator find(const QString &key) const;
        void insert(const MapKey &key, ParseNode *node);
        int remove(const MapKey &key);
        void clear();
        ParseNode* &operator[](const MapKey &key);
        ParseNode *operator[](const MapKey &key) const;
        ParseNodeMap toMap() const;

private:
        ParseNodeMap m_map;
    };
}

Q_DECLARE_METATYPE(Command::MapNode*)

#endif /* MAPNODE_H */
