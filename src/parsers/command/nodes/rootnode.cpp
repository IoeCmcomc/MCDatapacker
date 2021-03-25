#include "rootnode.h"

#include "literalnode.h"

static int _ = qRegisterMetaType<QSharedPointer<Command::RootNode> >();

Command::RootNode::RootNode(int pos) : Command::ParseNode(pos) {
}

QString Command::RootNode::toString() const {
    /*return QString("RootNode(%1)").arg(m_children.length()); */
    QString ret = QString("RootNode[%1](").arg(m_children.length());

    for (const auto &child: m_children) {
        ret += child->toString();
        if (child != m_children.last())
            ret += ", ";
    }
    return ret + ')';
}

void Command::RootNode::accept(Command::NodeVisitor *visitor) {
    for (const auto &child: m_children) {
        child->accept(visitor);
    }
    visitor->visit(this);
}

bool Command::RootNode::isEmpty() {
    return m_children.isEmpty();
}

int Command::RootNode::size() {
    return m_children.size();
}

void Command::RootNode::append(QSharedPointer<ParseNode> node) {
    m_children.append(node);
}

void Command::RootNode::prepend(QSharedPointer<ParseNode> node) {
    m_children.prepend(node);
}

void Command::RootNode::remove(int i) {
    m_children.remove(i);
}

void Command::RootNode::clear() {
    m_children.clear();
}

QSharedPointer<Command::ParseNode> &Command::RootNode::operator[](int index) {
    return m_children[index];
}

const QSharedPointer<Command::ParseNode> Command::RootNode::operator[](
    int index) const {
    return m_children[index];
}

QVector<QSharedPointer<Command::ParseNode> > Command::RootNode::children() const
{
    return m_children;
}

