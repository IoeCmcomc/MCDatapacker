#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "argumentnode.h"

namespace Command {
    class EntityNode : public ArgumentNode
    {
public:
        explicit EntityNode(int length);

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
            m_isValid = node->isValid();
            m_ptr     = std::forward<T>(node);
        }

protected:
        NodePtr m_ptr;

        explicit EntityNode(ParserType parserType, int length,
                            const NodePtr &ptr);

        bool m_singleOnly = false; // Parser property
        bool m_playerOnly = false; // Parser property
    };

    class GameProfileNode final : public EntityNode {
public:
        explicit GameProfileNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;
    };

    class ScoreHolderNode final : public EntityNode {
public:
        explicit ScoreHolderNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        bool isAll() const;
        void setAll(bool all);

private:
        bool m_all = false;
    };

    class InvertibleNode final : public ParseNode {
public:
        explicit InvertibleNode(bool inverted = false);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        bool isInverted() const;
        void setInverted(bool inverted);

        NodePtr getNode() const;
        template <typename T>
        typename std::enable_if_t<std::is_assignable_v<NodePtr, T>, void>
        setNode(T &&node) {
            setLength(node->length());
            m_isValid = node->isValid();
            m_ptr     = std::forward<T>(node);
        }

private:
        NodePtr m_ptr;
        bool m_inverted = false;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Entity)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, GameProfile)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ScoreHolder)
}

#endif /* ENTITYNODE_H */
