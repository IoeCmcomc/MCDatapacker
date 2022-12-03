#include "entitynode.h"

static const int _EntityNode =
    qRegisterMetaType<QSharedPointer<Command::EntityNode> >();

Command::EntityNode::EntityNode(int pos)
    : Command::ArgumentNode(pos, -1, "minecraft:entity") {
    m_ptrVari.setValue(nullptr);
}

Command::EntityNode::EntityNode(QSharedPointer<Command::StringNode> other)
    : Command::ArgumentNode(other->pos(),
                            other->length(), "minecraft:entity") {
    m_ptrVari.setValue(other);
    m_singleOnly = true;
}

Command::EntityNode::EntityNode(
    QSharedPointer<Command::TargetSelectorNode> other)
    : Command::ArgumentNode(other->pos(),
                            other->length(), "minecraft:entity") {
    m_ptrVari.setValue(other);

    using Variable = Command::TargetSelectorNode::Variable;
    switch (other->variable()) {
        case Variable::A: {
            m_playerOnly = true;
            break;
        }

        case Variable::E:
            break;

        case Variable::P:
        case Variable::R: {
            m_playerOnly = true;
            m_singleOnly = true;
            break;
        }

        case Variable::S: {
            m_singleOnly = true;
            break;
        }
    }
}

Command::EntityNode::EntityNode(QSharedPointer<Command::UuidNode> other)
    : Command::ArgumentNode(other->pos(), other->length(), "minecraft:entity") {
    m_ptrVari.setValue(other);
    m_singleOnly = true;
}

QString Command::EntityNode::toString() const {
    QStringList items;

    if (m_singleOnly)
        items << "single";
    if (m_playerOnly)
        items << "player";
    QString ret = QString("EntityNode[%1](").arg(items.join(", "));
    if (m_ptrVari.isValid() && !m_ptrVari.isNull()) {
        ret += castPtrVari()->toString();
    }
    return ret + ')';
}

bool Command::EntityNode::isVaild() const {
    return ParseNode::isVaild() && m_ptrVari.isValid();
}

void Command::EntityNode::accept(Command::NodeVisitor *visitor,
                                 Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (m_ptrVari.isValid() && !m_ptrVari.isNull())
        castPtrVari()->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

bool Command::EntityNode::playerOnly() const {
    return m_playerOnly;
}

void Command::EntityNode::setPlayerOnly(bool playerOnly) {
    m_playerOnly = playerOnly;
}

QVariant Command::EntityNode::ptrVari() const {
    return m_ptrVari;
}

void Command::EntityNode::setPtrVari(const QVariant &PtrVari) {
    m_ptrVari = PtrVari;
}

/*!
 * \brief Cast the shared pointer stored in the QVariant
 * to a QSharedPointer<ParseNode>.
 */
QSharedPointer<Command::ParseNode> Command::EntityNode::castPtrVari() const {
    if (m_ptrVari.canConvert<QSharedPointer<TargetSelectorNode> >()) {
        return qSharedPointerCast<ParseNode>(
            m_ptrVari.value<QSharedPointer<TargetSelectorNode> >());
    } else if (m_ptrVari.canConvert<QSharedPointer<StringNode> >()) {
        return qSharedPointerCast<ParseNode>(
            m_ptrVari.value<QSharedPointer<StringNode> >());
    } else if (m_ptrVari.canConvert<QSharedPointer<UuidNode> >()) {
        return qSharedPointerCast<ParseNode>(
            m_ptrVari.value<QSharedPointer<UuidNode> >());
    } else {
        qFatal("Cannot cast value in EntityNode to a QSharedPointer<ParseNode>");
    }
}

bool Command::EntityNode::singleOnly() const {
    return m_singleOnly;
}

void Command::EntityNode::setSingleOnly(bool singleOnly) {
    m_singleOnly = singleOnly;
}

static const int _GameProfileNode =
    qRegisterMetaType<QSharedPointer<Command::GameProfileNode> >();

Command::GameProfileNode::GameProfileNode(int pos)
    : Command::EntityNode(pos) {
    setParserId("minecraft:game_profile");
}

Command::GameProfileNode::GameProfileNode(Command::EntityNode *other)
    : Command::EntityNode(other->pos()) {
    setLength(other->length());
    setPtrVari(other->ptrVari());
    setParserId("minecraft:game_profile");
}

QString Command::GameProfileNode::toString() const {
    return EntityNode::toString().replace(0, 9, "GameProfileNode");
}

void Command::GameProfileNode::accept(Command::NodeVisitor *visitor,
                                      Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (ptrVari().isValid() && !ptrVari().isNull())
        castPtrVari()->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

static const int _ScoreHolderNode =
    qRegisterMetaType<QSharedPointer<Command::ScoreHolderNode> >();

Command::ScoreHolderNode::ScoreHolderNode(int pos)
    : Command::EntityNode(pos) {
    setParserId("minecraft:game_profile");
}

Command::ScoreHolderNode::ScoreHolderNode(Command::EntityNode *other)
    : Command::EntityNode(other->pos()) {
    setLength(other->length());
    setPtrVari(other->ptrVari());
    setSingleOnly(other->singleOnly());
    setPlayerOnly(other->playerOnly());
    setParserId("minecraft:score_holder");
}

QString Command::ScoreHolderNode::toString() const {
    if (isAll())
        return "ScoreHolderNode(*)";
    else
        return EntityNode::toString().replace(0, 10, "ScoreHolderNode");
}

void Command::ScoreHolderNode::accept(Command::NodeVisitor *visitor,
                                      Command::NodeVisitor::Order order) {
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (ptrVari().isValid() && !ptrVari().isNull())
        castPtrVari()->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

bool Command::ScoreHolderNode::isAll() const {
    return m_all;
}

void Command::ScoreHolderNode::setAll(bool all) {
    m_all = all;
}

static const int _EntityArgumentValueNode =
    qRegisterMetaType<QSharedPointer<Command::EntityArgumentValueNode> >();

Command::EntityArgumentValueNode::EntityArgumentValueNode(
    QSharedPointer<ParseNode> valNode, bool negative)
    : Command::ParseNode(valNode->pos(), valNode->length()), m_value(valNode) {
    setNegative(negative);
}

QString Command::EntityArgumentValueNode::toString() const {
    return (m_value) ? ((m_negative) ? ('!' +
                                        m_value->toString()) : m_value->toString())
    : "EntityArgumentValueNode()";
}

bool Command::EntityArgumentValueNode::isVaild() const {
    return ParseNode::isVaild() && m_value;
}

void Command::EntityArgumentValueNode::accept(Command::NodeVisitor *visitor,
                                              Command::NodeVisitor::Order order)
{
    if (order == NodeVisitor::Order::Preorder)
        visitor->visit(this);
    if (m_value)
        m_value->accept(visitor, order);
    if (order == NodeVisitor::Order::Postorder)
        visitor->visit(this);
}

bool Command::EntityArgumentValueNode::isNegative() const {
    return m_negative;
}

void Command::EntityArgumentValueNode::setNegative(bool negative) {
    m_negative = negative;
}

QSharedPointer<Command::ParseNode> Command::EntityArgumentValueNode::value()
const {
    return m_value;
}

void Command::EntityArgumentValueNode::setValue(QSharedPointer<ParseNode> value)
{
    m_value = value;
}
