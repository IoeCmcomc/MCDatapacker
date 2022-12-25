#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "singlevaluenode.h"
#include "literalnode.h"

namespace Command {
    class StringNode : public SingleValueNode<ArgumentNode, QString,
                                              ArgumentNode::ParserType::String>
    {
public:
        using SingleValueNode::SingleValueNode;

        static QSharedPointer<StringNode> fromLiteralNode(LiteralNode *node);

        void accept(NodeVisitor *visitor, VisitOrder) override;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::StringNode>);

#endif /* STRINGNODE_H */
