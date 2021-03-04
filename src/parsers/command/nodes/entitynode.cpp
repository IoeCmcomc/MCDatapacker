#include "entitynode.h"

static const int _EntityNode =
    qRegisterMetaType<QSharedPointer<Command::EntityNode> >();

Command::EntityNode::EntityNode(int pos)
    : Command::ArgumentNode(pos, -1, "minecraft:entity") {
    m_PtrVari.setValue(nullptr);
}

Command::EntityNode::EntityNode(QSharedPointer<Command::StringNode> other)
    : Command::ArgumentNode(other->pos(),
                            other->length(), "minecraft:entity") {
    m_PtrVari.setValue(other);
    m_singleOnly = true;
}

Command::EntityNode::EntityNode(
    QSharedPointer<Command::TargetSelectorNode> other)
    : Command::ArgumentNode(other->pos(),
                            other->length(), "minecraft:entity") {
    m_PtrVari.setValue(other);

    using Variable = Command::TargetSelectorNode::Variable;
    switch (other->variable()) {
    case Variable::A: {
        m_playerOnly = true;
        break;
    }

    case Variable::E:
        break;

    case Variable::P: {
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
    m_PtrVari.setValue(other);
    m_singleOnly = true;
}

QString Command::EntityNode::toString() const {
    QStringList items;

    if (m_singleOnly)
        items << "single";
    if (m_playerOnly)
        items << "player";
    QString ret = QString("EntityNode[%1](").arg(items.join(", "));
    if (m_PtrVari.isValid() && !m_PtrVari.isNull()) {
        QSharedPointer<ParseNode> argNode = nullptr;
        if (m_PtrVari.canConvert<QSharedPointer<TargetSelectorNode> >()) {
            argNode = qSharedPointerCast<ParseNode>(
                m_PtrVari.value<QSharedPointer<TargetSelectorNode> >());
        } else if (m_PtrVari.canConvert<QSharedPointer<StringNode> >()) {
            argNode = qSharedPointerCast<ParseNode>(
                m_PtrVari.value<QSharedPointer<StringNode> >());
        } else if (m_PtrVari.canConvert<QSharedPointer<UuidNode> >()) {
            argNode = qSharedPointerCast<ParseNode>(
                m_PtrVari.value<QSharedPointer<UuidNode> >());
        } else {
            qFatal(
                "Cannot cast value in EntityNode to QSharedPointer<ParseNode>");
        }
        ret += argNode->toString();
    }
    return ret + ')';
}

bool Command::EntityNode::isVaild() const {
    return ParseNode::isVaild() && m_PtrVari.isValid();
}

bool Command::EntityNode::playerOnly() const {
    return m_playerOnly;
}

void Command::EntityNode::setPlayerOnly(bool playerOnly) {
    m_playerOnly = playerOnly;
}

QVariant Command::EntityNode::PtrVari() const {
    return m_PtrVari;
}

void Command::EntityNode::setPtrVari(const QVariant &PtrVari) {
    m_PtrVari = PtrVari;
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
    setPtrVari(other->PtrVari());
    setParserId("minecraft:game_profile");
}

QString Command::GameProfileNode::toString() const {
    return EntityNode::toString().replace(0, 9, "GameProfileNode");
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
    setPtrVari(other->PtrVari());
    setSingleOnly(other->singleOnly());
    setPlayerOnly(other->playerOnly());
    setParserId("minecraft:score_holder");
}

QString Command::ScoreHolderNode::toString() const {
    if (isAll())
        return "ScoreHolderNode(*)";
    else
        return EntityNode::toString().replace(0, 9, "ScoreHolderNode");
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
    : Command::ParseNode(valNode->pos(), valNode->length()) {
    m_value = valNode;
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
