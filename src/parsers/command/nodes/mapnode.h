#ifndef MAPNODE_H
#define MAPNODE_H

#include "stringnode.h"

namespace Command {
    class KeyNode : public StringNode {
public:
        using StringNode::StringNode;
        void accept(NodeVisitor *visitor, VisitOrder order) final;
    };

    using KeyPtr = QSharedPointer<KeyNode>;

    template<typename KeyType = KeyPtr, typename ValueType = NodePtr>
    class PairNode : public ParseNode, public QPair<KeyType, ValueType> {
public:
        PairNode(KeyType key, ValueType node) : ParseNode(
                ParseNode::Kind::Container, 0) {
            Q_ASSERT(key != nullptr);
            m_isValid    = key->isValid() && node && node->isValid();
            this->first  = std::move(key);
            this->second = std::move(node);
        }
    };

    class MapNode : public ParseNode {
public:
        using Pair = QSharedPointer<PairNode<> >;

        using Pairs = QVector<Pair>;

        explicit MapNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        int size() const;
        bool isEmpty() const;
        bool contains(const QString &key) const;
        Pairs::const_iterator find(const QString &key) const;
        void insert(KeyPtr key, NodePtr node);
        void clear();
        PairNode<> inline * constLast() const {
            return m_pairs.constLast().data();
        }
        Pairs pairs() const;

private:
        Pairs m_pairs;
    };
}

#endif /* MAPNODE_H */
