#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "parsenode.h"

#include <deque>

namespace Command {
    class RootNode : public ParseNode
    {
public:
        using Nodes = std::deque<NodePtr>;

        explicit RootNode(int nodeLength = 0);

        void accept(NodeVisitor *visitor, VisitOrder order) final;

        bool isEmpty() const;
        int size() const;

        void append(NodePtr node);
        void prepend(NodePtr node);
        void remove(int i);
        void clear();

        Nodes children() const;

private:
        Nodes m_children;
    };

    DECLARE_TYPE_ENUM(ParseNode::Kind, Root)
}

#endif /* ROOTNODE_H */
