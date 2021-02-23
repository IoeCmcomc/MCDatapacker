#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "stringnode.h"
#include "targetselectornode.h"
#include "uuidnode.h"

namespace Command {
    class EntityNode : public ArgumentNode
    {
        Q_OBJECT
public:
        EntityNode(QObject *parent, int pos);
        EntityNode(QObject *parent, StringNode *other);
        EntityNode(QObject *parent, TargetSelectorNode *other);
        EntityNode(QObject *parent, UuidNode *other);
        virtual QString toString() const override;
        bool isVaild() const override;

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
        Q_OBJECT
public:
        GameProfileNode(QObject *parent, int pos);
        GameProfileNode(EntityNode *other);
        QString toString() const override;
    };

    class ScoreHolderNode final : public EntityNode {
        Q_OBJECT
public:
        ScoreHolderNode(QObject *parent, int pos);
        ScoreHolderNode(EntityNode *other);
        QString toString() const override;
        bool isAll() const;
        void setAll(bool all);

private:
        bool m_all = false;
    };

    class EntityArgumentValueNode final : public ParseNode {
        Q_OBJECT
public:
        EntityArgumentValueNode(ParseNode *valNode, bool negative = false);
        QString toString() const override;
        bool isVaild() const override;

        bool isNegative() const;
        void setNegative(bool negative);

        Command::ParseNode *value() const;
        void setValue(ParseNode *value);
private:
        bool m_negative    = false;
        ParseNode *m_value = nullptr;
    };
}

Q_DECLARE_METATYPE(Command::EntityNode*)
Q_DECLARE_METATYPE(Command::GameProfileNode*)
Q_DECLARE_METATYPE(Command::ScoreHolderNode*)
Q_DECLARE_METATYPE(Command::EntityArgumentValueNode*)

#endif /* ENTITYNODE_H */
