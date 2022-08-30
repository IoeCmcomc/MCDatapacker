#include "itemstacknode.h"

static const int _ItemStackNode =
    qRegisterMetaType<QSharedPointer<Command::ItemStackNode> >();

Command::ItemStackNode::ItemStackNode(int pos, const QString &nspace,
                                      const QString &id)
    : Command::ResourceLocationNode(pos, nspace, id) {
}

QString Command::ItemStackNode::toString() const {
    auto ret =
        QString("ItemStackNode(%1)").arg(ResourceLocationNode::fullId());

    if (m_nbt)
        ret += '{' + m_nbt->toString() + '}';
    return ret;
}

bool Command::ItemStackNode::isVaild() const {
    return ResourceLocationNode::isVaild() && m_nbt;
}

void Command::ItemStackNode::accept(Command::NodeVisitor *visitor,
                                    Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (m_nbt)
        m_nbt->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

QSharedPointer<Command::NbtCompoundNode> Command::ItemStackNode::nbt() const {
    return m_nbt;
}

void Command::ItemStackNode::setNbt(QSharedPointer<NbtCompoundNode> nbt) {
    m_nbt = nbt;
}

static const int _ItemPredicateNode =
    qRegisterMetaType<QSharedPointer<Command::ItemPredicateNode> >();

Command::ItemPredicateNode::ItemPredicateNode(int pos, const QString &nspace,
                                              const QString &id)
    : Command::ItemStackNode(pos, nspace, id) {
    setParserId("minecraft:item_predicate");
}

Command::ItemPredicateNode::ItemPredicateNode(Command::ItemStackNode *other)
    : Command::ItemStackNode(other->pos(), other->nspace(), other->id()) {
    setParserId("minecraft:item_predicate");
    setLength(other->length());
}

QString Command::ItemPredicateNode::toString() const {
    return ItemStackNode::toString().replace(0, 13, "ItemPredicateNode");
}

void Command::ItemPredicateNode::accept(Command::NodeVisitor *visitor,
                                        Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (nbt())
        nbt()->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}
