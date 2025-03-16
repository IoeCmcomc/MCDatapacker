#ifndef LISTNODE_H
#define LISTNODE_H

#include "parsenode.h"

namespace Command {
    template<class T>
    class ListlikeNode {
public:
        ListlikeNode() = default;

        bool isEmpty() {
            return m_vector.isEmpty();
        }
        int size() {
            return m_vector.size();
        }
        void append(QSharedPointer<T> node) {
            m_vector.append(std::move(node));
        }
        void insert(int i, QSharedPointer<T> node) {
            m_vector.insert(i, std::move(node));
        }
        void remove(int i) {
            m_vector.remove(i);
        }
        void clear() {
            m_vector.clear();
        }
        QSharedPointer<T> &operator[](int index) {
            return m_vector[index];
        };
        const QSharedPointer<T> &operator[](int index) const {
            return m_vector[index];
        };
        QVector<QSharedPointer<T> > children() const {
            return m_vector;
        }
        QVector<QSharedPointer<T> > &children() {
            return m_vector;
        }

protected:
        QVector<QSharedPointer<T> > m_vector;

        template<class U>
        void append(U&& node, bool &isValid) {
            isValid = m_vector.empty() ? node->isValid()
                                    : isValid && node->isValid();
            m_vector.push_back(std::forward<U>(node));
        }
    };

    /*!
     * \class ListNode
     * \brief A general-purpose parse node containing a list of nodes.
     *
     */
    class ListNode : public ParseNode, public ListlikeNode<ParseNode> {
public:
        explicit ListNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        template<class T>
        void append(T&& node) {
            ListlikeNode::append(std::forward<T>(node), m_isValid);
        }
    };
}

#endif // LISTNODE_H
