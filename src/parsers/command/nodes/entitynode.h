#ifndef ENTITYNODE_H
#define ENTITYNODE_H

//#include "stringnode.h"
//#include "targetselectornode.h"
//#include "singlevaluenode.h"

#include "argumentnode.h"

namespace Command {
    class EntityNode : public ArgumentNode
    {
public:
        explicit EntityNode(int length);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool singleOnly() const;
        void setSingleOnly(bool singleOnly);

        bool playerOnly() const;
        void setPlayerOnly(bool playerOnly);

        NodePtr getNode() const;
        template <typename T>
        typename std::enable_if_t<std::is_assignable_v<NodePtr, T> >
        setNode(T &&node) {
            setLength(node->length());
            m_ptr = std::forward<T>(node);
        }

protected:
        NodePtr m_ptr;

        explicit EntityNode(ParserType parserType, int length,
                            const NodePtr &ptr);

private:
        bool m_singleOnly = false; // Parser property
        bool m_playerOnly = false; // Parser property
    };

    class GameProfileNode final : public EntityNode {
public:
        explicit GameProfileNode(int length);
        explicit GameProfileNode(EntityNode *other);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    class ScoreHolderNode final : public EntityNode {
public:
        explicit ScoreHolderNode(int length);
        explicit ScoreHolderNode(EntityNode *other);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool isAll() const;
        void setAll(bool all);

private:
        bool m_all = false;
    };

    class EntityArgumentValueNode final : public ParseNode {
public:
        using ArgPtr = QSharedPointer<ArgumentNode>;

        explicit EntityArgumentValueNode(bool negative = false);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool isNegative() const;
        void setNegative(bool negative);

        ArgPtr getNode() const;
        template <typename T>
        typename std::enable_if_t<std::is_assignable_v<ArgPtr, T> >
        setNode(T &&node) {
            m_ptr = std::forward<T>(node);
        }

private:
        ArgPtr m_ptr;
        bool m_negative = false;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Entity)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, GameProfile)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ScoreHolder)
}

#endif /* ENTITYNODE_H */
