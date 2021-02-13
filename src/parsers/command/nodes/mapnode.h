#ifndef MAPNODE_H
#define MAPNODE_H

#include "parsenode.h"

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
    class MapNode : public ParseNode
    {
        Q_OBJECT
public:
        MapNode(QObject *parent, int pos, int length = 0);

        QString toString() const override;

        int size() const;
        bool contains(const QString &key) const;
        void insert(const QString &key, ParseNode *node);
        int remove(const QString &key);
        void clear();
        Command::ParseNode* &operator[](const QString &key);
        Command::ParseNode* operator[](const QString &key) const;


private:
        QMap<QString, ParseNode*> m_map;
    };
}

Q_DECLARE_METATYPE(Command::MapNode*)

#endif /* MAPNODE_H */
