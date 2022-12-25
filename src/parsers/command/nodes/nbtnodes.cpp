#include "nbtnodes.h"
#include "../visitors/nodevisitor.h"

static bool       _  = TypeRegister<Command::NbtNode>::init();
const static bool __ =
    TypeRegister<Command::NbtStringNode>::init();

namespace Command {
    NbtNode::NbtNode(TagType tagType, int length)
        : ArgumentNode(ParserType::NbtTag, length), m_tagType(tagType) {
    }

    NbtNode::NbtNode(ParserType parserType, int length)
        : ArgumentNode(parserType, length) {
    }

    NbtNode::NbtNode(ParserType parserType, const QString &text)
        : ArgumentNode(parserType, text) {
    }

    NbtNode::NbtNode(ParserType parserType, TagType tagType, int length)
        : ArgumentNode(parserType, length), m_tagType(tagType) {
    }

    DEFINE_ACCEPT_METHOD(NbtNode)

    NbtNode::TagType NbtNode::tagType() const {
        return m_tagType;
    }

    DEFINE_ACCEPT_METHOD(NbtByteNode)
    DEFINE_ACCEPT_METHOD(NbtDoubleNode)
    DEFINE_ACCEPT_METHOD(NbtFloatNode)
    DEFINE_ACCEPT_METHOD(NbtIntNode)
    DEFINE_ACCEPT_METHOD(NbtLongNode)
    DEFINE_ACCEPT_METHOD(NbtShortNode)
    DEFINE_ACCEPT_METHOD(NbtStringNode)

#define DEFINE_ARRAY_NBTNODE(Class)                                            \
        static const int _ ## Class =                                          \
            qRegisterMetaType<QSharedPointer<Class> >();                       \
        void Command::Class::accept(NodeVisitor * visitor, VisitOrder order) { \
            if (order == VisitOrder::LetTheVisitorDecide) {                    \
                visitor->visit(this);                                          \
                return;                                                        \
            }                                                                  \
            if (order == VisitOrder::Preorder) {                               \
                visitor->visit(this);                                          \
            }                                                                  \
            for (const auto &elem: qAsConst(m_vector)) {                       \
                elem->accept(visitor, order);                                  \
            }                                                                  \
            if (order == VisitOrder::Postorder) {                              \
                visitor->visit(this);                                          \
            }                                                                  \
        }                                                                      \

    DEFINE_ARRAY_NBTNODE(NbtByteArrayNode)
    DEFINE_ARRAY_NBTNODE(NbtIntArrayNode)
    DEFINE_ARRAY_NBTNODE(NbtLongArrayNode)


    static const int _NbtListNode =
        qRegisterMetaType<QSharedPointer<NbtListNode> >();

    NbtListNode::NbtListNode(int length) : NbtNode(TagType::List, length) {
    }

    void NbtListNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        for (const auto &node: qAsConst(m_vector)) {
            node->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    void NbtListNode::append(QSharedPointer<NbtNode> node) {
        if (isEmpty())
            m_prefix = node->tagType();
        if (node->tagType() == m_prefix)
            m_vector << node;
        else
            qWarning() << "Incompatible node type";
    }
    NbtNode::TagType NbtListNode::prefix() const {
        return m_prefix;
    }
    void NbtListNode::setPrefix(TagType prefix) {
        m_prefix = prefix;
    }


    static bool _2 = TypeRegister<NbtCompoundNode>::init();

    NbtCompoundNode::NbtCompoundNode(int length)
        : NbtNode(ParserType::NbtCompoundTag, TagType::Compound, length) {
    }

    void NbtCompoundNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        for (auto i = m_pairs.cbegin(); i != m_pairs.cend(); ++i) {
            i->get()->first->accept(visitor, order);
            i->get()->second->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }
    int NbtCompoundNode::size() const {
        return m_pairs.size();
    }

    bool NbtCompoundNode::isEmpty() const {
        return m_pairs.isEmpty();
    }

    bool NbtCompoundNode::contains(const QString &key) const {
        return std::any_of(m_pairs.cbegin(), m_pairs.cend(),
                           [&key](const Pair& pair) {
            return pair->first->value() == key;
        });
    }

    NbtCompoundNode::Pairs::const_iterator NbtCompoundNode::find(
        const QString &key) const {
        return std::find_if(m_pairs.cbegin(), m_pairs.cend(),
                            [&key](const Pair& pair) {
            return pair->first->value() == key;
        });
    }

    void NbtCompoundNode::insert(KeyPtr key, NbtPtr node) {
        m_pairs << Pair::create(key, node);
    }

    void NbtCompoundNode::clear() {
        m_pairs.clear();
    }

    NbtCompoundNode::Pairs NbtCompoundNode::pairs() const {
        return m_pairs;
    }
}
