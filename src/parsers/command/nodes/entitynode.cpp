#include "entitynode.h"

static const int _EntityNode = qRegisterMetaType<Command::EntityNode*>();

Command::EntityNode::EntityNode(QObject *parent, int pos)
    : Command::ArgumentNode(parent, pos, -1, "minecraft:entity") {
    m_PtrVari.setValue(nullptr);
}

Command::EntityNode::EntityNode(QObject *parent, Command::StringNode *other)
    : Command::ArgumentNode(parent, other->pos(),
                            other->length(), "minecraft:entity") {
    m_PtrVari.setValue(other);
    m_singleOnly = true;
}

Command::EntityNode::EntityNode(QObject *parent,
                                Command::TargetSelectorNode *other)
    : Command::ArgumentNode(parent, other->pos(),
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

Command::EntityNode::EntityNode(QObject *parent, Command::UuidNode *other)
    : Command::ArgumentNode(parent, other->pos(),
                            other->length(), "minecraft:entity") {
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
        auto *argNode = qvariant_cast<ParseNode*>(m_PtrVari);
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
    qRegisterMetaType<Command::GameProfileNode*>();

Command::GameProfileNode::GameProfileNode(QObject *parent, int pos)
    : Command::EntityNode(parent, pos) {
    setParserId("minecraft:game_profile");
}

Command::GameProfileNode::GameProfileNode(Command::EntityNode *other)
    : Command::EntityNode(other->parent(), other->pos()) {
    setLength(other->length());
    setPtrVari(other->PtrVari());
    setParserId("minecraft:game_profile");
}

QString Command::GameProfileNode::toString() const {
    return EntityNode::toString().replace(0, 9, "GameProfileNode");
}

static const int _ScoreHolderNode =
    qRegisterMetaType<Command::ScoreHolderNode*>();

Command::ScoreHolderNode::ScoreHolderNode(QObject *parent, int pos)
    : Command::EntityNode(parent, pos) {
    setParserId("minecraft:game_profile");
}

Command::ScoreHolderNode::ScoreHolderNode(Command::EntityNode *other)
    : Command::EntityNode(other->parent(), other->pos()) {
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
    qRegisterMetaType<Command::EntityArgumentValueNode*>();

Command::EntityArgumentValueNode::EntityArgumentValueNode(
    Command::ParseNode *valNode, bool negative)
    : Command::ParseNode(valNode->parent(), valNode->pos(), valNode->length()) {
    m_value = valNode;
    valNode->setParent(this);
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

Command::ParseNode *Command::EntityArgumentValueNode::value() const {
    return m_value;
}

void Command::EntityArgumentValueNode::setValue(ParseNode *value) {
    m_value = value;
}
