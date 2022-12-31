#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "parsenode.h"

namespace Command {
    class RootNode : public ParseNode
    {
public:
        using Nodes = std::list<NodePtr>;

        explicit RootNode(int nodeLength = 0);

        void accept(NodeVisitor *visitor, VisitOrder order) override;

        bool isEmpty();
        int size();

        void append(NodePtr node);
        void prepend(NodePtr node);
        void remove(int i);
        void clear();

        NodePtr &operator[](int index);
        const NodePtr operator[](int index) const;

        Nodes children() const;

private:
        Nodes m_children;
    };

    DECLARE_TYPE_ENUM(ParseNode::Kind, Root)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::RootNode>)

#endif /* ROOTNODE_H */
