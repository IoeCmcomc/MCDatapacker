#include "argumentnode.h"
#include "../visitors/nodevisitor.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::ArgumentNode> >();

namespace Command {
    void ArgumentNode::accept(Command::NodeVisitor *visitor,
                              Command::VisitOrder) {
        visitor->visit(this);
    }

    ArgumentNode::ParserType ArgumentNode::parserType() const {
        return m_parserType;
    }
}
