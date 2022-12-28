#include "schemaliteralnode.h"

namespace Command::Schema {
    LiteralNode::LiteralNode() : Node(Node::Kind::Literal) {
    }

    void from_json(const json &j, LiteralNode *&n) {
        n = new LiteralNode();
        n->_from_json(j);
    }
}
