#include "nbtnodes.h"

static const int _NbtNode =
    qRegisterMetaType<QSharedPointer<Command::NbtNode> >();

Command::NbtNode::NbtNode(int pos, int length, const QString &parserId)
    : Command::ArgumentNode(pos, length, parserId) {
}

QString Command::NbtNode::toString() const {
    return "NbtNode()";
}

#define DEFINE_PRIMITIVE_TAG_NBTNODE(Class, Tag, ValueType)              \
    static const int _ ## Class =                                        \
        qRegisterMetaType<QSharedPointer<Command::Class> >();            \
    Command::Class::Class(int pos, int length, ValueType value)          \
        : Command::NbtNode(pos, length), tags::Tag(value) {              \
    }                                                                    \
    QString Command::Class::toString() const {                           \
        return #Class + QString("(%1)").arg(QString::fromStdString(      \
                                                std::to_string(*this))); \
    }                                                                    \
    nbt::tag_id Command::Class::id() const noexcept {                    \
        return tags::Tag::id();                                          \
    }

DEFINE_PRIMITIVE_TAG_NBTNODE(NbtByteNode, byte_tag, char)
DEFINE_PRIMITIVE_TAG_NBTNODE(NbtDoubleNode, double_tag, double)
DEFINE_PRIMITIVE_TAG_NBTNODE(NbtFloatNode, float_tag, float)
DEFINE_PRIMITIVE_TAG_NBTNODE(NbtIntNode, int_tag, int)
DEFINE_PRIMITIVE_TAG_NBTNODE(NbtLongNode, long_tag, int64_t)
DEFINE_PRIMITIVE_TAG_NBTNODE(NbtShortNode, short_tag, short)

#define DEFINE_ARRAY_NBTNODE(Class, TagId)                     \
    static const int _ ## Class =                              \
        qRegisterMetaType<QSharedPointer<Command::Class> >();  \
    Command::Class::Class(int pos, int length)                 \
        : Command::NbtNode(pos, length, "minecraft:nbt_tag") { \
    }                                                          \
    QString Command::Class::toString() const {                 \
        QStringList items;                                     \
        for (const auto &node: m_vector)                       \
        items << node->toString();                             \
        return #Class + QString("(%1)").arg(items.join(", ")); \
    }                                                          \
    nbt::tag_id Command::Class::id() const noexcept {          \
        return nbt::tag_id::TagId;                             \
    }

DEFINE_ARRAY_NBTNODE(NbtByteArrayNode, tag_bytearray)
DEFINE_ARRAY_NBTNODE(NbtIntArrayNode, tag_intarray)
DEFINE_ARRAY_NBTNODE(NbtLongArrayNode, tag_longarray)

static const int _NbtStringNode =
    qRegisterMetaType<QSharedPointer<Command::NbtStringNode> >();
Command::NbtStringNode::NbtStringNode(int pos,
                                      const QString &value,
                                      bool isQuote)
    : Command::NbtNode(pos, value.length() + (isQuote * 2)) {
    setValue(value);
}
QString Command::NbtStringNode::toString() const {
    return QString("NbtStringNode(%1)").arg(m_value);
}
QString Command::NbtStringNode::value() const {
    return m_value;
}
void Command::NbtStringNode::setValue(const QString &v) {
    m_value = v;
}
nbt::tag_id Command::NbtStringNode::id() const noexcept {
    return nbt::tag_id::tag_string;
}


static const int _NbtListNode =
    qRegisterMetaType<QSharedPointer<Command::NbtListNode> >();
Command::NbtListNode::NbtListNode(int pos, int length)
    : Command::NbtNode(pos, length, "minecraft:nbt_tag") {
}
QString Command::NbtListNode::toString() const {
    QStringList items;

    for (const auto &node: m_vector) {
        items << node->toString();
    }
    return QString("NbtListNode(%1)").arg(items.join(", "));
}
nbt::tag_id Command::NbtListNode::id() const noexcept {
    return nbt::tag_id::tag_list;
}
void Command::NbtListNode::append(QSharedPointer<NbtNode> node) {
    if (isEmpty())
        m_prefix = node->id();
    if (node->id() == m_prefix)
        m_vector.append(node);
    else
        qWarning() << "Incompatible node type:" << node->toString();
}
nbt::tag_id Command::NbtListNode::prefix() const {
    return m_prefix;
}
void Command::NbtListNode::setPrefix(const nbt::tag_id &prefix) {
    m_prefix = prefix;
}


const static int _ =
    qRegisterMetaType<QSharedPointer<Command::NbtCompoundNode> >();
Command::NbtCompoundNode::NbtCompoundNode(int pos, int length)
    : Command::NbtNode(pos, length, "minecraft:nbt_compound_tag") {
}
QString Command::NbtCompoundNode::toString() const {
    QStringList itemReprs;

    for (auto i = m_map.cbegin(); i != m_map.cend(); ++i) {
        itemReprs << QString("%1: %2").arg(i.key().text,
                                           i.value()->toString());
    }
    return "MapNode(" + itemReprs.join(", ") + ')';
}
int Command::NbtCompoundNode::size() const {
    return m_map.size();
}
bool Command::NbtCompoundNode::contains(const QString &key) const {
    auto it = m_map.cbegin();

    while (it != m_map.cend()) {
        if (it.key().text == key)
            return true;

        ++it;
    }
    return false;
}
bool Command::NbtCompoundNode::contains(const MapKey &key)
const {
    return m_map.contains(key);
}
Command::NbtNodeMap::const_iterator Command::NbtCompoundNode::find(
    const QString &key)
const {
    auto it = m_map.cbegin();

    while (it != m_map.cend()) {
        if (it.key().text == key)
            return it;

        ++it;
    }
    return m_map.cend();
}
void Command::NbtCompoundNode::insert(const MapKey &key,
                                      QSharedPointer<NbtNode> node) {
    m_map.insert(key, node);
}
int Command::NbtCompoundNode::remove(const MapKey &key) {
    return m_map.remove(key);
}
void Command::NbtCompoundNode::clear() {
    m_map.clear();
}
QSharedPointer<Command::NbtNode> &Command::NbtCompoundNode::operator[](
    const Command::MapKey &key) {
    return m_map[key];
}
const QSharedPointer<Command::NbtNode> Command::NbtCompoundNode::operator[](
    const Command::MapKey &key)
const {
    return m_map[key];
}
Command::NbtNodeMap Command::NbtCompoundNode::toMap() const {
    return m_map;
}
