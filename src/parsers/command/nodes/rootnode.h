#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "parsenode.h"

namespace Command {
    class RootNode : public ParseNode
    {
        Q_OBJECT
public:
        explicit RootNode(QObject *parent = nullptr);
        ~RootNode();

        QString toString() const;

        bool isEmpty();
        int size();

        void append(Command::ParseNode *node);
        void prepend(Command::ParseNode *node);
        void remove(int i);
        void removeNode(Command::ParseNode *node);
        void clear();

        Command::ParseNode *operator[](int index);
        Command::ParseNode *operator[](int index) const;

        QVector<Command::ParseNode *> children() const;

private:
        QVector<Command::ParseNode*> m_children;
    };
}

Q_DECLARE_METATYPE(Command::RootNode*);

#endif /* ROOTNODE_H */
