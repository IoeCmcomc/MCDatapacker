#ifndef ARGUMENTNODE_H
#define ARGUMENTNODE_H

#include "parsenode.h"

namespace Command {
    class ArgumentNode : public ParseNode
    {
public:
        explicit ArgumentNode(int pos, int length, QString parserId);

        virtual QString toString() const override;
        QString parserId() const;

        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;
protected:
        void setParserId(const QString &parserId);

private:
        QString m_parserId;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ArgumentNode>);

#endif /* ARGUMENTNODE_H */
