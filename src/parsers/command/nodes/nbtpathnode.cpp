#include "nbtpathnode.h"
#include "../visitors/nodevisitor.h"

static const int _NbtPathStepnode =
    qRegisterMetaType<QSharedPointer<Command::NbtPathStepNode> >();
const static bool _ = TypeRegister<Command::NbtPathNode>::init();

namespace Command {
    NbtPathStepNode::NbtPathStepNode(int length)
        : ParseNode(ParseNode::Kind::Container, length) {
    }

    bool NbtPathStepNode::isValid() const {
        bool ret = ParseNode::isValid();

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

            default: {
                Q_UNREACHABLE();
                return false;
            }
        }
    }

    void NbtPathStepNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        switch (m_type) {
            case Type::Root: {
                m_filter->accept(visitor, order);
                break;
            }

            case Type::Key: {
                return m_name->accept(visitor, order);

                break;
            }

            case Type::Index: {
                if (m_filter)
                    m_filter->accept(visitor, order);
                else if (m_index)
                    m_index->accept(visitor, order);
            }
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    QSharedPointer<StringNode> NbtPathStepNode::name() const {
        return m_name;
    }

    void NbtPathStepNode::setName(QSharedPointer<StringNode> name) {
        m_name = name;
        setType(Type::Key);
    }

    QSharedPointer<IntegerNode> NbtPathStepNode::index() const {
        return m_index;
    }

    void NbtPathStepNode::setIndex(QSharedPointer<IntegerNode> index) {
        m_index = index;
        setType(Type::Index);
    }

    QSharedPointer<NbtCompoundNode> NbtPathStepNode::filter()
    const {
        return m_filter;
    }

    void NbtPathStepNode::setFilter(QSharedPointer<NbtCompoundNode> filter) {
        m_filter = filter;
    }

    NbtPathStepNode::Type NbtPathStepNode::type() const {
        return m_type;
    }

    void NbtPathStepNode::setType(const Type &type) {
        m_type = type;
    }

    NbtPathNode::NbtPathNode(int length)
        : ArgumentNode(ArgumentNode::ParserType::NbtPath, length) {
    }

    bool NbtPathNode::isValid() const {
        return ArgumentNode::isValid() && !m_steps.isEmpty();
    }

    void NbtPathNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        for (const auto &step: qAsConst(m_steps))
            step->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    int NbtPathNode::size() const {
        return m_steps.length();
    }

    void NbtPathNode::append(QSharedPointer<NbtPathStepNode> node) {
        m_steps.append(node);
    }

    void NbtPathNode::remove(int i) {
        m_steps.remove(i);
    }

    void NbtPathNode::clear() {
        m_steps.clear();
    }

    QSharedPointer<NbtPathStepNode> NbtPathNode::last() const {
        return m_steps.last();
    }

    QSharedPointer<NbtPathStepNode> &NbtPathNode::operator[](int index) {
        return m_steps[index];
    }

    const QSharedPointer<NbtPathStepNode> NbtPathNode::operator[](
        int index) const {
        return m_steps[index];
    }

    NbtPathNode::Steps NbtPathNode::steps()  const {
        return m_steps;
    }
}
