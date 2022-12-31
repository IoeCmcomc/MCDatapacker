#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "stringnode.h"
#include "targetselectornode.h"
#include "singlevaluenode.h"

namespace Command {
    class EntityNode : public ArgumentNode
    {
public:
        explicit EntityNode(int length);
        explicit EntityNode(const QSharedPointer<StringNode> &other);
        explicit EntityNode(const QSharedPointer<TargetSelectorNode> &other);
        explicit EntityNode(const QSharedPointer<UuidNode> &other);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool singleOnly() const;
        void setSingleOnly(bool singleOnly);

        bool playerOnly() const;
        void setPlayerOnly(bool playerOnly);

        NodePtr getNode() const;
        void setNode(const NodePtr &ptr);

protected:
        NodePtr m_ptr = nullptr;

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
        explicit EntityArgumentValueNode(QSharedPointer<ArgumentNode> valNode,
                                         bool negative = false);
        explicit EntityArgumentValueNode(bool negative = false);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool isNegative() const;
        void setNegative(bool negative);

        QSharedPointer<ArgumentNode> getNode() const;
        void setNode(QSharedPointer<ArgumentNode> value);
private:
        QSharedPointer<ArgumentNode> m_ptr = nullptr;
        bool m_negative                    = false;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Entity)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, GameProfile)
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, ScoreHolder)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::EntityNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::GameProfileNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ScoreHolderNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::EntityArgumentValueNode>)

#endif /* ENTITYNODE_H */
