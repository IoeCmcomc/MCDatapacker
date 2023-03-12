#ifndef MAPNODE_H
#define MAPNODE_H

#include "stringnode.h"

namespace Command {
    class KeyNode : public StringNode {
public:
        using StringNode::StringNode;
        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    using KeyPtr = QSharedPointer<KeyNode>;

    template<typename T>
    class PairNode : public ParseNode, public QPair<KeyPtr, T> {
public:
        PairNode(KeyPtr key, T node) : ParseNode(
                ParseNode::Kind::Container, 0) {
            m_isValid    = key->isValid() && node->isValid();
            this->first  = std::move(key);
            this->second = std::move(node);
        }
    };

    class MapNode : public ParseNode {
public:
        using Pair = QSharedPointer<PairNode<NodePtr> >;

        using Pairs = QVector<Pair>;

        explicit MapNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        int size() const;
        bool isEmpty() const;
        bool contains(const QString &key) const;
        Pairs::const_iterator find(const QString &key) const;
        void insert(KeyPtr key, NodePtr node);
        void clear();
        PairNode<NodePtr> inline * constLast() const {
            return m_pairs.constLast().data();
        }
        Pairs pairs() const;

private:
        Pairs m_pairs;
    };
}

#endif /* MAPNODE_H */
