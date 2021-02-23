#ifndef NBTPATHNODE_H
#define NBTPATHNODE_H

#include "integernode.h"
#include "nbtnodes.h"
#include "stringnode.h"

namespace Command {
    class NbtPathStepNode : public ParseNode {
        Q_OBJECT
public:
        enum class Type : unsigned char {
            Root,
            Key,
            Index,
        };
        Q_ENUM(Type)

        NbtPathStepNode(QObject *parent, int pos);
        QString toString() const override;
        bool isVaild() const override;

        StringNode *name() const;
        void setName(StringNode *name);

        IntegerNode *index() const;
        void setIndex(IntegerNode *index);

        NbtCompoundNode *filter() const;
        void setFilter(NbtCompoundNode *filter);

        Type type() const;
        void setType(const Type &type);

        bool hasTrailingDot() const;
        void setHasTrailingDot(bool hasTrailingDot);

private:
        StringNode *m_name        = nullptr;
        IntegerNode *m_index      = nullptr;
        NbtCompoundNode *m_filter = nullptr;
        Type m_type               = Type::Root;
        bool m_hasTrailingDot     = false;
    };

    class NbtPathNode : public ArgumentNode
    {
        Q_OBJECT
public:
        NbtPathNode(QObject *parent, int pos);
        QString toString() const override;
        bool isVaild() const override;

        bool isEmpty();
        int size();

        void append(Command::NbtPathStepNode *node);
        void remove(int i);
        void clear();

        Command::NbtPathStepNode *last() const;
        Command::NbtPathStepNode *operator[](int index);
        Command::NbtPathStepNode *operator[](int index) const;

        QVector<NbtPathStepNode *> steps() const;

private:
        QVector<NbtPathStepNode*> m_steps;
    };
}

Q_DECLARE_METATYPE(Command::NbtPathStepNode*)
Q_DECLARE_METATYPE(Command::NbtPathNode*)

#endif /* NBTPATHNODE_H */
