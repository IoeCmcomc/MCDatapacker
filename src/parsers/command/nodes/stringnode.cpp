#include "stringnode.h"
#include "../visitors/nodevisitor.h"

const static bool _ = TypeRegister<Command::StringNode>::init();

namespace Command {
    QSharedPointer<StringNode> StringNode::fromLiteralNode(
        LiteralNode *node) {
        if (node) {
            return QSharedPointer<StringNode>::create(node->text());
        }
        return nullptr;
    }

    DEFINE_ACCEPT_METHOD(StringNode)
}
