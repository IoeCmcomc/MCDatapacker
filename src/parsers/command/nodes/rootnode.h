#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "parsenode.h"

namespace Command {
    class RootNode : public ParseNode
    {
public:
        explicit RootNode(QObject *parent = nullptr);

        QString toString() const;

        bool isEmpty();
        int size();

        void append(Command::ParseNode *node);
        void remove(int i);
        void removeNode(Command::ParseNode *node);

        Command::ParseNode *operator[](int index);
        Command::ParseNode *operator[](int index) const;
        Command::RootNode &operator<<(Command::ParseNode *node);


        QVector<Command::ParseNode *> children() const;

private:
        QVector<Command::ParseNode*> m_children;
    };
}

#endif /* ROOTNODE_H */
