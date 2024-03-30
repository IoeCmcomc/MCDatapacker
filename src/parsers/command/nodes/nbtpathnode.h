#ifndef NBTPATHNODE_H
#define NBTPATHNODE_H

#include "argumentnode.h"

namespace Command {
    class StringNode;
    class IntegerNode;
    class NbtCompoundNode;

    class NbtPathStepNode : public ParseNode {
public:
        enum class Type {
            Root,
            Key,
            Index,
        };

        explicit NbtPathStepNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        QSharedPointer<StringNode> name() const;
        void setName(QSharedPointer<StringNode> name);

        QSharedPointer<IntegerNode> index() const;
        void setIndex(QSharedPointer<IntegerNode> index);

        QSharedPointer<NbtCompoundNode> filter() const;
        void setFilter(QSharedPointer<NbtCompoundNode> filter);

        Type type() const;
        void setType(const Type &type);

private:
        QSharedPointer<StringNode> m_name;
        QSharedPointer<IntegerNode> m_index;
        QSharedPointer<NbtCompoundNode> m_filter;
        Type m_type = Type::Root;
    };

    class NbtPathNode : public ArgumentNode {
public:
        using Steps = QVector<QSharedPointer<NbtPathStepNode> >;

        explicit NbtPathNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        bool isEmpty();
        int size() const;

        void append(QSharedPointer<NbtPathStepNode> node);
        void remove(int i);
        void clear();

        QSharedPointer<NbtPathStepNode> last() const;
        QSharedPointer<NbtPathStepNode> &operator[](int index);
        const QSharedPointer<NbtPathStepNode> operator[](int index) const;

        Steps steps() const;

private:
        Steps m_steps;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, NbtPath)
}

#endif /* NBTPATHNODE_H */
