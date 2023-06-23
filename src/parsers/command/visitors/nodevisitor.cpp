#include "nodevisitor.h"

#include "../nodes/parsenode.h"

void Command::NodeVisitor::startVisiting(ParseNode *node) {
    node->accept(this, m_order);
}
