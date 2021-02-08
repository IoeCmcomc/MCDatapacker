#include "rootnode.h"

static int _ = qRegisterMetaType<Command::RootNode*>();

Command::RootNode::RootNode(QObject *parent) : Command::ParseNode(parent) {
}

Command::RootNode::~RootNode() {
    qDeleteAll(m_children);
}

QString Command::RootNode::toString() const {
    /*return QString("RootNode(%1)").arg(m_children.length()); */
    QString ret = QString("RootNode[%1](").arg(m_children.length());

    for (const auto *child: m_children) {
        ret += child->toString();
        if (child != m_children.last())
            ret += ", ";
    }
    return ret + ')';
}

bool Command::RootNode::isEmpty() {
    return m_children.isEmpty();
}

int Command::RootNode::size() {
    return m_children.size();
}

void Command::RootNode::append(Command::ParseNode *node) {
    m_children.append(node);
}

void Command::RootNode::prepend(Command::ParseNode *node) {
    m_children.prepend(node);
}

void Command::RootNode::remove(int i) {
    delete m_children[i];
    m_children.remove(i);
}

void Command::RootNode::removeNode(Command::ParseNode *node) {
    if (int i = m_children.indexOf(node); i > -1)
        m_children.remove(i);
}

void Command::RootNode::clear() {
    qDeleteAll(m_children);
    m_children.clear();
}

Command::ParseNode *Command::RootNode::operator[](int index) {
    return m_children[index];
}

Command::ParseNode *Command::RootNode::operator[](int index) const {
    return m_children[index];
}

Command::RootNode &Command::RootNode::operator<<(Command::ParseNode *node) {
    append(node);
    return *this;
}

QVector<Command::ParseNode *> Command::RootNode::children() const {
    return m_children;
}

