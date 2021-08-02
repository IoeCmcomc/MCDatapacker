#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "stringnode.h"
#include "targetselectornode.h"
#include "uuidnode.h"

namespace Command {
    class EntityNode : public ArgumentNode
    {
public:
        EntityNode(int pos);
        EntityNode(QSharedPointer<StringNode> other);
        EntityNode(QSharedPointer<TargetSelectorNode> other);
        EntityNode(QSharedPointer<UuidNode> other);
        virtual QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            QSharedPointer<ParseNode> argNode = nullptr;

            qDebug() << m_PtrVari;
            if (m_PtrVari.canConvert<QSharedPointer<TargetSelectorNode> >()) {
                argNode = qSharedPointerCast<ParseNode>(
                    m_PtrVari.value<QSharedPointer<TargetSelectorNode> >());
            } else if (m_PtrVari.canConvert<QSharedPointer<StringNode> >()) {
                argNode = qSharedPointerCast<ParseNode>(
                    m_PtrVari.value<QSharedPointer<StringNode> >());
            } else if (m_PtrVari.canConvert<QSharedPointer<UuidNode> >()) {
                argNode = qSharedPointerCast<ParseNode>(
                    m_PtrVari.value<QSharedPointer<UuidNode> >());
            } else {
                qFatal(
                    "Cannot cast value in EntityNode to a QSharedPointer<ParseNode>");
            }
            argNode->accept(visitor);
            visitor->visit(this);
        }

        bool singleOnly() const;
        void setSingleOnly(bool singleOnly);

        bool playerOnly() const;
        void setPlayerOnly(bool playerOnly);

        QVariant PtrVari() const;

protected:
        void setPtrVari(const QVariant &PtrVari);

private:
        QVariant m_PtrVari;
        bool m_singleOnly = false;
        bool m_playerOnly = false;
    };

    class GameProfileNode final : public EntityNode {
public:
        GameProfileNode(int pos);
        GameProfileNode(EntityNode *other);
        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            EntityNode::accept(visitor);
            visitor->visit(this);
        }
    };

    class ScoreHolderNode final : public EntityNode {
public:
        ScoreHolderNode(int pos);
        ScoreHolderNode(EntityNode *other);
        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            EntityNode::accept(visitor);
            visitor->visit(this);
        }
        bool isAll() const;
        void setAll(bool all);

private:
        bool m_all = false;
    };

    class EntityArgumentValueNode final : public ParseNode {
public:
        EntityArgumentValueNode(QSharedPointer<ParseNode> valNode,
                                bool negative = false);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }

        bool isNegative() const;
        void setNegative(bool negative);

        QSharedPointer<ParseNode> value() const;
        void setValue(QSharedPointer<ParseNode> value);
private:
        bool m_negative                   = false;
        QSharedPointer<ParseNode> m_value = nullptr;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::EntityNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::GameProfileNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::ScoreHolderNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::EntityArgumentValueNode>)

#endif /* ENTITYNODE_H */
