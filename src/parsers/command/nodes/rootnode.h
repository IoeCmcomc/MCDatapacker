#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "parsenode.h"

namespace Command {
    class RootNode : public ParseNode
    {
public:
        explicit RootNode(int pos);

        QString toString() const override;
        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;

        bool isEmpty();
        int size();

        void append(QSharedPointer<ParseNode> node);
        void prepend(QSharedPointer<ParseNode> node);
        void remove(int i);
        void clear();

        QSharedPointer<Command::ParseNode> &operator[](int index);
        const QSharedPointer<Command::ParseNode> operator[](int index) const;

        QVector<QSharedPointer<ParseNode> > children() const;

private:
        QVector<QSharedPointer<ParseNode> > m_children;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::RootNode>);

#endif /* ROOTNODE_H */
