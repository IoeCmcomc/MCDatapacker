#ifndef UUIDNODE_H
#define UUIDNODE_H

#include "argumentnode.h"

#include <QUuid>

namespace Command {
    class UuidNode : public ArgumentNode
    {
public:
        UuidNode(int pos, int length, const QString &uuidStr);
        UuidNode(int pos, const QString &uuidStr);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }

        QUuid uuid() const;
        void setUuid(const QUuid &uuid);

private:
        QUuid m_uuid;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::UuidNode>);

#endif /* UUIDNODE_H */
