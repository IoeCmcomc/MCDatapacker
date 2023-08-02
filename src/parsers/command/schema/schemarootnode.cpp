#include "schemarootnode.h"

namespace Command::Schema {
    RootNode::RootNode() : Node(Node::Kind::Root) {
    }

    void from_json(const json &j, RootNode *&n) {
        n = new RootNode();
        n->_from_json(j);
    }
}
