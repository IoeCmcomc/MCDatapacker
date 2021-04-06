#ifndef NBTPATHNODE_H
#define NBTPATHNODE_H

#include "integernode.h"
#include "nbtnodes.h"
#include "stringnode.h"

namespace Command {
    class NbtPathStepNode : public ParseNode {
        Q_GADGET
public:
        enum class Type : unsigned char {
            Root,
            Key,
            Index,
        };
        Q_ENUM(Type)

        NbtPathStepNode(int pos);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }

        QSharedPointer<StringNode> name() const;
        void setName(QSharedPointer<StringNode> name);

        QSharedPointer<IntegerNode> index() const;
        void setIndex(QSharedPointer<IntegerNode> index);

        QSharedPointer<NbtCompoundNode> filter() const;
        void setFilter(QSharedPointer<NbtCompoundNode> filter);

        Type type() const;
        void setType(const Type &type);

        bool hasTrailingDot() const;
        void setHasTrailingDot(bool hasTrailingDot);

private:
        QSharedPointer<StringNode> m_name        = nullptr;
        QSharedPointer<IntegerNode> m_index      = nullptr;
        QSharedPointer<NbtCompoundNode> m_filter = nullptr;
        Type m_type                              = Type::Root;
        bool m_hasTrailingDot                    = false;
    };

    class NbtPathNode : public ArgumentNode
    {
public:
        NbtPathNode(int pos);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            for (const auto &step: qAsConst(m_steps))
                step->accept(visitor);
            visitor->visit(this);
        }

        bool isEmpty();
        int size();

        void append(QSharedPointer<NbtPathStepNode> node);
        void remove(int i);
        void clear();

        QSharedPointer<NbtPathStepNode> last() const;
        QSharedPointer<NbtPathStepNode> &operator[](int index);
        const QSharedPointer<NbtPathStepNode> operator[](int index) const;

        QVector<QSharedPointer<NbtPathStepNode> > steps() const;

private:
        QVector<QSharedPointer<NbtPathStepNode> > m_steps;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::NbtPathStepNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtPathNode>)

#endif /* NBTPATHNODE_H */
