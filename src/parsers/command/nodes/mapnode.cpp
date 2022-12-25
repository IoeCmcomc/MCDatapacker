#include "mapnode.h"
#include "../visitors/nodevisitor.h"

const static int _ = qRegisterMetaType<QSharedPointer<Command::MapNode> >();

namespace Command {
    DEFINE_ACCEPT_METHOD(KeyNode)

    MapNode::MapNode(int length) : ParseNode(Kind::Container, length) {
    }

    void MapNode::accept(NodeVisitor *visitor, VisitOrder order) {
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

    int MapNode::size() const {
        return m_pairs.size();
    }

    bool MapNode::isEmpty() const {
        return m_pairs.isEmpty();
    }

    bool MapNode::contains(const QString &key) const {
        return std::any_of(m_pairs.cbegin(), m_pairs.cend(),
                           [&key](const Pair& pair) {
            return pair->first->value() == key;
        });
    }

    MapNode::Pairs::const_iterator MapNode::find(const QString &key) const {
        return std::find_if(m_pairs.cbegin(), m_pairs.cend(),
                            [&key](const Pair& pair) {
            return pair->first->value() == key;
        });
    }

    void MapNode::insert(KeyPtr key, NodePtr node) {
        m_pairs << Pair::create(key, node);
    }

    void MapNode::clear() {
        m_pairs.clear();
    }

    MapNode::Pairs MapNode::pairs() const {
        return m_pairs;
    }
}
