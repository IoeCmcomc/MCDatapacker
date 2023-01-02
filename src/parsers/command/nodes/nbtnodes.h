#ifndef NBTNODES_H
#define NBTNODES_H

#include "argumentnode.h"
#include "mapnode.h"
#include "singlevaluenode.h"

namespace Command {
    class NbtNode : public ArgumentNode {
public:
        enum class TagType {
            Unknown,
            Byte,
            Double,
            Float,
            Int,
            Long,
            Short,
            ByteArray,
            IntArray,
            LongArray,
            String,
            List,
            Compound,
        };

        virtual void accept(NodeVisitor *visitor, VisitOrder) override;

        TagType tagType() const;

protected:
        TagType m_tagType = TagType::Unknown;

        explicit NbtNode(TagType tagType, int length);
        explicit NbtNode(ParserType parserType, int length);
        explicit NbtNode(ParserType parserType, const QString &text);
        explicit NbtNode(ParserType parserType, TagType tagType, int length);
    };

    DECLARE_TYPE_ENUM_FULL(NbtNode, ArgumentNode::ParserType, NbtTag)

#define DECLARE_PRIMITIVE_TAG_NBTNODE(Name, T)                                               \
        class Nbt ## Name ## Node : public SingleValueNode<NbtNode, T,                       \
                                                           ArgumentNode::ParserType::NbtTag> \
        {                                                                                    \
public:                                                                                      \
            Nbt ## Name ## Node(const QString &text, const T &value)                         \
                : SingleValueNode(text, value) {                                             \
                m_tagType = TagType::Name;                                                   \
            };                                                                               \
            void accept(NodeVisitor * visitor, VisitOrder) override;                         \
        };                                                                                   \

    DECLARE_PRIMITIVE_TAG_NBTNODE(Byte, int8_t)
    DECLARE_PRIMITIVE_TAG_NBTNODE(Double, double)
    DECLARE_PRIMITIVE_TAG_NBTNODE(Float, float)
    DECLARE_PRIMITIVE_TAG_NBTNODE(Int, int)
    DECLARE_PRIMITIVE_TAG_NBTNODE(Long, int64_t)
    DECLARE_PRIMITIVE_TAG_NBTNODE(Short, short)
    //DECLARE_PRIMITIVE_TAG_NBTNODE(String, QString)

#undef DECLARE_PRIMITIVE_TAG_NBTNODE

    class NbtStringNode : public SingleValueNode<NbtNode, QString,
                                                 ArgumentNode::ParserType::NbtTag>
    {
public:
        NbtStringNode(const QString &text, const QString &value)
            : SingleValueNode(text, value) {
            m_tagType = TagType::String;
        };
        explicit NbtStringNode(const QString &text)
            : SingleValueNode(text, text) {
            m_tagType = TagType::String;
        };
        void accept(NodeVisitor *visitor, VisitOrder) override;
    };

    template<class T>
    class NbtListlikeNode {
public:
        NbtListlikeNode() = default;
        bool isEmpty() {
            return m_vector.isEmpty();
        }
        int size() {
            return m_vector.size();
        }
        virtual void append(QSharedPointer<T> node) {
            m_vector.append(node);
        }
        void insert(int i, QSharedPointer<T> node) {
            m_vector.insert(i, node);
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

protected:
        QVector<QSharedPointer<T> > m_vector;
    };

#define DECLARE_ARRAY_NBTNODE(Name, ValueType)                             \
        class Nbt ## Name ## Node : public NbtNode,                        \
            public NbtListlikeNode<ValueType> {                            \
public:                                                                    \
            explicit Nbt ## Name ## Node(int length)                       \
                : NbtNode(ParserType::NbtTag, TagType::Name, length) {};   \
            void accept(NodeVisitor * visitor, VisitOrder order) override; \
        };                                                                 \

    DECLARE_ARRAY_NBTNODE(ByteArray, NbtByteNode)
    DECLARE_ARRAY_NBTNODE(IntArray, NbtIntNode)
    DECLARE_ARRAY_NBTNODE(LongArray, NbtLongNode)

#undef DECLARE_ARRAY_NBTNODE

    using NbtPtr = QSharedPointer<NbtNode>;

    class NbtListNode : public NbtNode, public NbtListlikeNode<NbtNode> {
public:
        explicit NbtListNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
        void append(NbtPtr node) override;

        TagType prefix() const;
        void setPrefix(TagType prefix);

private:
        TagType m_prefix = TagType::Unknown;
    };

    class NbtCompoundNode : public NbtNode {
public:

        using Pair  = QSharedPointer<PairNode<NbtPtr> >;
        using Pairs = QVector<Pair>;

        explicit NbtCompoundNode(int length = 0);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        int size() const;
        bool isEmpty() const;
        bool contains(const QString &key) const;
        Pairs::const_iterator find(const QString &key) const;
        void insert(KeyPtr key, NbtPtr node);
        void clear();
        PairNode<NbtPtr> inline * constLast() const {
            return m_pairs.constLast().data();
        }
        Pairs pairs() const;

private:
        Pairs m_pairs;
    };

    DECLARE_TYPE_ENUM_FULL(NbtCompoundNode, ArgumentNode::ParserType,
                           NbtCompoundTag)
}

#endif /* NBTNODES_H */
