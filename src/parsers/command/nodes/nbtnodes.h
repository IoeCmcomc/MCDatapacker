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
        Q_OBJECT
public:
        NbtNode(QObject *parent, int pos, int length,
                const QString &parserId = "minecraft:nbt_tag");
        virtual QString toString() const override;
        virtual nbt::tag_id id() const noexcept;
    };

#define DECLARE_PRIMITIVE_TAG_NBTNODE(Class, Tag, ValueType)                    \
    class Class : public NbtNode, tags::Tag {                                   \
        Q_OBJECT                                                                \
public:                                                                         \
        explicit Class(QObject * parent, int pos, int length, ValueType value); \
        QString toString() const override;                                      \
        ValueType value() const;                                                \
        void setValue(ValueType v);                                             \
        nbt::tag_id id() const noexcept override;                               \
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
        virtual void append(T *node) {
            m_vector.append(node);
        }
        void insert(int i, T *node) {
            m_vector.insert(i, node);
        }
        void remove(int i) {
            m_vector.remove(i);
        }
        void removeNode(T *node) {
            if (int i = m_vector.indexOf(node); i > -1)
                m_vector.remove(i);
        }
        void clear() {
            m_vector.clear();
        }
        T *operator[](int index) {
            return m_vector[index];
        };
        T *operator[](int index) const {
            return m_vector[index];
        };
protected:
        QVector<T*> m_vector;
    };

#define DECLARE_ARRAY_NBTNODE(Class, ValueType)                       \
    class Class : public NbtNode, public NbtListlikeNode<ValueType> { \
        Q_OBJECT                                                      \
public:                                                               \
        explicit Class(QObject * parent, int pos, int length = 0);    \
        QString toString() const override;                            \
        nbt::tag_id id() const noexcept override;                     \
    };

    DECLARE_ARRAY_NBTNODE(NbtByteArrayNode, NbtByteNode)
    DECLARE_ARRAY_NBTNODE(NbtIntArrayNode, NbtIntNode)
    DECLARE_ARRAY_NBTNODE(NbtLongArrayNode, NbtLongNode)

#undef DECLARE_ARRAY_NBTNODE

    class NbtStringNode : public NbtNode {
        Q_OBJECT
public:
        explicit NbtStringNode(QObject * parent, int pos,
                               const QString &value);
        QString toString() const override;
        QString value() const;
        void setValue(const QString &v);
        nbt::tag_id id() const noexcept override;
private:
        QString m_value;
    };

    class NbtListNode : public NbtNode, public NbtListlikeNode<NbtNode> {
        Q_OBJECT
public:
        explicit NbtListNode(QObject * parent, int pos, int length = 0);
        QString toString() const override;
        void append(NbtNode* node) override;
        nbt::tag_id id() const noexcept override;
        nbt::tag_id prefix() const;
        void setPrefix(const nbt::tag_id &prefix);
private:
        nbt::tag_id m_prefix;
    };

    using NbtNodeMap = QMap<MapKey, NbtNode*>;

    class NbtCompoundNode : public NbtNode
    {
        Q_OBJECT
public:
        NbtCompoundNode(QObject *parent, int pos, int length = 0);

        QString toString() const override;

        int size() const;
        bool contains(const QString &key) const;
        bool contains(const MapKey &key) const;
        NbtNodeMap::const_iterator find(const QString &key) const;
        void insert(const MapKey &key, NbtNode *node);
        int remove(const MapKey &key);
        void clear();
        NbtNode* &operator[](const MapKey &key);
        NbtNode *operator[](const MapKey &key) const;
        NbtNodeMap toMap() const;

private:
        NbtNodeMap m_map;
    };
}

Q_DECLARE_METATYPE(Command::NbtNode*)
Q_DECLARE_METATYPE(Command::NbtByteNode*)
Q_DECLARE_METATYPE(Command::NbtDoubleNode*)
Q_DECLARE_METATYPE(Command::NbtFloatNode*)
Q_DECLARE_METATYPE(Command::NbtIntNode*)
Q_DECLARE_METATYPE(Command::NbtLongNode*)
Q_DECLARE_METATYPE(Command::NbtShortNode*)

Q_DECLARE_METATYPE(Command::NbtByteArrayNode*)
Q_DECLARE_METATYPE(Command::NbtIntArrayNode*)
Q_DECLARE_METATYPE(Command::NbtLongArrayNode*)

Q_DECLARE_METATYPE(Command::NbtStringNode*)
Q_DECLARE_METATYPE(Command::NbtListNode*)
Q_DECLARE_METATYPE(Command::NbtCompoundNode*)

#endif /* NBTNODES_H */
