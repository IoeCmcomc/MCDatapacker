#include "targetselectornode.h"

#include <QMetaEnum>

static const int _ =
    qRegisterMetaType<QSharedPointer<Command::TargetSelectorNode> >();

const QMap<Command::TargetSelectorNode::Variable, char>
Command::TargetSelectorNode::variableMap =
{   { Variable::A, 'a' },
    { Variable::E, 'e' },
    { Variable::P, 'p' },
    { Variable::R, 'r' },
    { Variable::S, 's' } };

Command::TargetSelectorNode::TargetSelectorNode(int pos)
    : Command::ParseNode(pos) {
}

QString Command::TargetSelectorNode::toString() const {
    auto ret = QString("TargetSelectorNode(@%1)").arg(variableMap[m_variable]);

    if (m_args)
        ret += '{' + m_args->toString() + '}';
    return ret;
}

void Command::TargetSelectorNode::accept(Command::NodeVisitor *visitor,
                                         Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (m_args) {
        m_args->accept(visitor, order);
    }
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

QSharedPointer<Command::MultiMapNode> Command::TargetSelectorNode::args() const
{
    return m_args;
}

void Command::TargetSelectorNode::setArgs(QSharedPointer<MultiMapNode> args) {
    m_args = args;
}

Command::TargetSelectorNode::Variable Command::TargetSelectorNode::variable()
const {
    return m_variable;
}

void Command::TargetSelectorNode::setVariable(const Variable &variable) {
    m_variable = variable;
}
