#include "nbtpathnode.h"
#include "../visitors/nodevisitor.h"

namespace Command {
    NbtPathStepNode::NbtPathStepNode(int length)
        : ParseNode(ParseNode::Kind::Container, length) {
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
                if (m_filter)
                    m_filter->accept(visitor, order);
                break;
            }

            case Type::Key: {
                if (m_name)
                    m_name->accept(visitor, order);
                if (m_filter)
                    m_filter->accept(visitor, order);

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
        m_isValid = name->isValid();
        m_name    = std::move(name);
        setType(Type::Key);
    }

    QSharedPointer<IntegerNode> NbtPathStepNode::index() const {
        return m_index;
    }

    void NbtPathStepNode::setIndex(QSharedPointer<IntegerNode> index) {
        m_isValid &= index->isValid();
        m_index    = std::move(index);
        setType(Type::Index);
    }

    QSharedPointer<NbtCompoundNode> NbtPathStepNode::filter()
    const {
        return m_filter;
    }

    void NbtPathStepNode::setFilter(QSharedPointer<NbtCompoundNode> filter) {
        m_isValid &= filter->isValid();
        m_filter   = std::move(filter);
    }

    NbtPathStepNode::Type NbtPathStepNode::type() const {
        return m_type;
    }

    void NbtPathStepNode::setType(const Type &type) {
        m_type    = type;
        m_isValid = true;
    }

    NbtPathNode::NbtPathNode(int length)
        : ArgumentNode(ArgumentNode::ParserType::NbtPath, length) {
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
        m_isValid =
            m_steps.empty() ? node->isValid() : (m_isValid && node->isValid());
        m_steps.append(std::move(node));
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
