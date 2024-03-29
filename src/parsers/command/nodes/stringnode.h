#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "singlevaluenode.h"

namespace Command {
    class StringNode : public SingleValueNode<ArgumentNode, QString,
                                              ArgumentNode::ParserType::String>
    {
public:
        using SingleValueNode::SingleValueNode;

        void accept(NodeVisitor *visitor, VisitOrder) override;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, String)
}

#endif /* STRINGNODE_H */
