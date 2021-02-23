#include "targetselectornode.h"

#include <QMetaEnum>

static const int _ = qRegisterMetaType<Command::TargetSelectorNode*>();

const QMap<Command::TargetSelectorNode::Variable, char>
Command::TargetSelectorNode::variableMap =
{   { Command::TargetSelectorNode::Variable::A, 'a' },
    { Command::TargetSelectorNode::Variable::E, 'e' },
    { Command::TargetSelectorNode::Variable::P, 'p' },
    { Command::TargetSelectorNode::Variable::S, 's' } };

Command::TargetSelectorNode::TargetSelectorNode(QObject *parent, int pos)
    : Command::ParseNode(parent, pos) {
}

QString Command::TargetSelectorNode::toString() const {
    auto ret = QString("TargetSelectorNode(@%1)").arg(variableMap[m_variable]);

    if (m_args)
        ret += '{' + m_args->toString() + '}';
    return ret;
}

Command::MapNode *Command::TargetSelectorNode::args() const {
    return m_args;
}

void Command::TargetSelectorNode::setArgs(MapNode *args) {
    m_args = args;
}

Command::TargetSelectorNode::Variable Command::TargetSelectorNode::variable()
const {
    return m_variable;
}

void Command::TargetSelectorNode::setVariable(const Variable &variable) {
    m_variable = variable;
}
