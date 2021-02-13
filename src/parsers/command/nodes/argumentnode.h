#ifndef ARGUMENTNODE_H
#define ARGUMENTNODE_H

#include "parsenode.h"

namespace Command {
    class ArgumentNode : public ParseNode
    {
        Q_OBJECT
public:
        explicit ArgumentNode(QObject *parent,
                              int pos,
                              int length,
                              QString parserId);

        virtual QString toString() const override;
        QString parserId() const;
protected:
        void setParserId(const QString &parserId);

private:
        QString m_parserId;
    };
}

Q_DECLARE_METATYPE(Command::ArgumentNode*);

#endif /* ARGUMENTNODE_H */
