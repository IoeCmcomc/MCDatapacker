#include "targetselectornode.h"

#include <QMetaEnum>

static const int _ =
    qRegisterMetaType<QSharedPointer<Command::TargetSelectorNode> >();

const QMap<Command::TargetSelectorNode::Variable, char>
Command::TargetSelectorNode::variableMap =
{   { Command::TargetSelectorNode::Variable::A, 'a' },
    { Command::TargetSelectorNode::Variable::E, 'e' },
    { Command::TargetSelectorNode::Variable::P, 'p' },
    { Command::TargetSelectorNode::Variable::R, 'r' },
    { Command::TargetSelectorNode::Variable::S, 's' } };

Command::TargetSelectorNode::TargetSelectorNode(int pos)
    : Command::ParseNode(pos) {
}

QString Command::TargetSelectorNode::toString() const {
    auto ret = QString("TargetSelectorNode(@%1)").arg(variableMap[m_variable]);

    if (m_args)
        ret += '{' + m_args->toString() + '}';
    return ret;
}

QSharedPointer<Command::MapNode> Command::TargetSelectorNode::args() const {
    return m_args;
}

void Command::TargetSelectorNode::setArgs(QSharedPointer<MapNode> args) {
    m_args = args;
}

Command::TargetSelectorNode::Variable Command::TargetSelectorNode::variable()
const {
    return m_variable;
}

void Command::TargetSelectorNode::setVariable(const Variable &variable) {
    m_variable = variable;
}
