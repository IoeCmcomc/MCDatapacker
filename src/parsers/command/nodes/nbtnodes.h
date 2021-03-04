#ifndef NBTNODES_H
#define NBTNODES_H

#include "argumentnode.h"
#include "mapnode.h"
#include "nbt-cpp/include/nbt.hpp"

#include <iostream>
#include <sstream>

namespace Command {
    using namespace nbt;
    class NbtNode : public ArgumentNode
    {
public:
        NbtNode(int pos, int length,
                const QString &parserId = "minecraft:nbt_tag");
        virtual QString toString() const override;
        virtual nbt::tag_id id() const noexcept;
    };

#define DECLARE_PRIMITIVE_TAG_NBTNODE(Class, Tag, ValueType)  \
    class Class : public NbtNode, tags::Tag {                 \
public:                                                       \
        explicit Class(int pos, int length, ValueType value); \
        QString toString() const override;                    \
        ValueType value() const;                              \
        void setValue(ValueType v);                           \
        nbt::tag_id id() const noexcept override;             \
    };

    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtByteNode, byte_tag, char)
    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtDoubleNode, double_tag, double)
    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtFloatNode, float_tag, float)
    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtIntNode, int_tag, int)
    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtLongNode, long_tag, int64_t)
    DECLARE_PRIMITIVE_TAG_NBTNODE(NbtShortNode, short_tag, short)

#undef DECLARE_PRIMITIVE_TAG_NBTNODE

/*
      class AbstractNbtListlikeNode {
          virtual bool isEmpty() = 0;
          virtual int size();
          virtual void append(T *node);
          virtual void insert(int i, T *node);
          virtual void remove(int i);
          virtual void removeNode(T *node);
          virtual void clear();
          virtual T *operator[](int index);
          virtual T *operator[](int index) const;
          virtual NbtListlikeNode &operator<<(T *node);
      };
 */

    template<typename T>
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
protected:
        QVector<QSharedPointer<T> > m_vector;
    };

#define DECLARE_ARRAY_NBTNODE(Class, ValueType)                       \
    class Class : public NbtNode, public NbtListlikeNode<ValueType> { \
public:                                                               \
        explicit Class(int pos, int length = 0);                      \
        QString toString() const override;                            \
        nbt::tag_id id() const noexcept override;                     \
    };

    DECLARE_ARRAY_NBTNODE(NbtByteArrayNode, NbtByteNode)
    DECLARE_ARRAY_NBTNODE(NbtIntArrayNode, NbtIntNode)
    DECLARE_ARRAY_NBTNODE(NbtLongArrayNode, NbtLongNode)

#undef DECLARE_ARRAY_NBTNODE

    class NbtStringNode : public NbtNode {
public:
        explicit NbtStringNode(int pos, const QString &value);
        QString toString() const override;
        QString value() const;
        void setValue(const QString &v);
        nbt::tag_id id() const noexcept override;
private:
        QString m_value;
    };

    class NbtListNode : public NbtNode, public NbtListlikeNode<NbtNode> {
public:
        explicit NbtListNode(int pos, int length = 0);
        QString toString() const override;
        void append(QSharedPointer<NbtNode> node) override;
        nbt::tag_id id() const noexcept override;
        nbt::tag_id prefix() const;
        void setPrefix(const nbt::tag_id &prefix);
private:
        nbt::tag_id m_prefix;
    };

    using NbtNodeMap = QMap<MapKey, QSharedPointer<NbtNode> >;

    class NbtCompoundNode : public NbtNode
    {
public:
        NbtCompoundNode(int pos, int length = 0);

        QString toString() const override;

        int size() const;
        bool contains(const QString &key) const;
        bool contains(const MapKey &key) const;
        NbtNodeMap::const_iterator find(const QString &key) const;
        void insert(const MapKey &key, QSharedPointer<NbtNode> node);
        int remove(const MapKey &key);
        void clear();
        QSharedPointer<NbtNode> &operator[](const MapKey &key);
        const QSharedPointer<NbtNode> operator[](const MapKey &key) const;
        NbtNodeMap toMap() const;

private:
        NbtNodeMap m_map;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::NbtNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtByteNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtDoubleNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtFloatNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtIntNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtLongNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtShortNode>)

Q_DECLARE_METATYPE(QSharedPointer<Command::NbtByteArrayNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtIntArrayNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtLongArrayNode>)

Q_DECLARE_METATYPE(QSharedPointer<Command::NbtStringNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtListNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::NbtCompoundNode>)

#endif /* NBTNODES_H */
