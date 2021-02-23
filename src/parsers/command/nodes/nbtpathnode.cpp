#include "nbtpathnode.h"

#include <QMetaEnum>

static const int _NbtPathStepnode =
    qRegisterMetaType<Command::NbtPathStepNode*>();

Command::NbtPathStepNode::NbtPathStepNode(QObject *parent, int pos)
    : Command::ParseNode(parent, pos) {
}

QString Command::NbtPathStepNode::toString() const {
    QString ret = QString("NbtPathStepNode<%1>(").arg(
        QMetaEnum::fromType<NbtPathStepNode::Type>().valueToKey(static_cast<int>(
                                                                    m_type)));

    if (!isVaild())
        return ret += "Invaild)";

    switch (m_type) {
    case Type::Root: {
        ret += '{';
        if (m_filter)
            ret += m_filter->toString();
        ret += '}';
        break;
    }

    case Type::Key: {
        ret += m_name->toString();
        if (m_filter)
            ret += '{' + m_filter->toString() + '}';
        break;
    }

    case Type::Index: {
        ret += '[';
        if (m_filter)
            ret += '{' + m_filter->toString() + '}';
        else if (m_index)
            ret += m_index->toString();
        ret += ']';
        break;
    }
    }
    if (m_hasTrailingDot)
        ret += '.';
    return ret += ')';
}

bool Command::NbtPathStepNode::isVaild() const {
    bool ret = ParseNode::isVaild();

    if (!ret)
        return false;

    switch (m_type) {
    case Type::Root: {
        return ret && m_filter;
    }

    case Type::Key: {
        return ret && m_name;
    }

    case Type::Index: {
        return ret;
    }
    }
}

Command::StringNode *Command::NbtPathStepNode::name() const {
    return m_name;
}

void Command::NbtPathStepNode::setName(StringNode *name) {
    m_name = name;
    setType(Type::Key);
}

Command::IntegerNode *Command::NbtPathStepNode::index() const {
    return m_index;
}

void Command::NbtPathStepNode::setIndex(IntegerNode *index) {
    m_index = index;
    setType(Type::Index);
}

Command::NbtCompoundNode *Command::NbtPathStepNode::filter() const {
    return m_filter;
}

void Command::NbtPathStepNode::setFilter(NbtCompoundNode *filter) {
    m_filter = filter;
}

Command::NbtPathStepNode::Type Command::NbtPathStepNode::type() const {
    return m_type;
}

void Command::NbtPathStepNode::setType(const Type &type) {
    m_type = type;
}

bool Command::NbtPathStepNode::hasTrailingDot() const {
    return m_hasTrailingDot;
}

void Command::NbtPathStepNode::setHasTrailingDot(bool hasTrailingDot) {
    m_hasTrailingDot = hasTrailingDot;
}

static const int _NbtPathnode = qRegisterMetaType<Command::NbtPathNode*>();

Command::NbtPathNode::NbtPathNode(QObject *parent, int pos)
    : Command::ArgumentNode(parent, pos, -1, "minecraft:nbt_path") {
}

QString Command::NbtPathNode::toString() const {
    QStringList steps;

    for (const NbtPathStepNode *step: m_steps)
        steps += step->toString();
    return QString("NbtPathNode(%1)").arg(steps.join(", "));
}

bool Command::NbtPathNode::isVaild() const {
    return ArgumentNode::isVaild() && !m_steps.isEmpty();
}

void Command::NbtPathNode::append(Command::NbtPathStepNode *node) {
    m_steps.append(node);
}

void Command::NbtPathNode::remove(int i) {
    m_steps.remove(i);
}

void Command::NbtPathNode::clear() {
    m_steps.clear();
}

Command::NbtPathStepNode *Command::NbtPathNode::last() const {
    return m_steps.last();;
}

Command::NbtPathStepNode *Command::NbtPathNode::operator[](int index) {
    return m_steps[index];
}

Command::NbtPathStepNode *Command::NbtPathNode::operator[](int index) const {
    return m_steps[index];
}

QVector<Command::NbtPathStepNode*> Command::NbtPathNode::steps() const {
    return m_steps;
}
