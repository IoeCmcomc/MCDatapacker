#ifndef UUIDNODE_H
#define UUIDNODE_H

#include "argumentnode.h"

#include <QUuid>

namespace Command {
    class UuidNode : public ArgumentNode
    {
        Q_OBJECT
public:
        UuidNode(QObject *parent, int pos, QString uuidStr);
        QString toString() const override;
        bool isVaild() const override;

        QUuid uuid() const;
        void setUuid(const QUuid &uuid);

private:
        QUuid m_uuid;
    };
}

Q_DECLARE_METATYPE(Command::UuidNode*);

#endif /* UUIDNODE_H */
