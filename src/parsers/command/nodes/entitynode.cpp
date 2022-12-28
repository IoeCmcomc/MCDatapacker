#include "entitynode.h"
#include "../visitors/nodevisitor.h"

static bool _ = TypeRegister<Command::EntityNode>::init();

namespace Command {
    EntityNode::EntityNode(int length)
        : ArgumentNode(ParserType::Entity, length) {
    }

    EntityNode::EntityNode(const QSharedPointer<StringNode> &other)
        : ArgumentNode(ParserType::Entity, other->length()), m_ptr(other) {
    }

    EntityNode::EntityNode(const QSharedPointer<TargetSelectorNode> &other)
        : ArgumentNode(ParserType::Entity, other->length()), m_ptr(other) {
    }

    EntityNode::EntityNode(const QSharedPointer<UuidNode> &other)
        : ArgumentNode(ParserType::Entity, other->length()), m_ptr(other),
        m_singleOnly(true) {
    }

    bool EntityNode::isValid() const {
        return ParseNode::isValid() && m_ptr;
    }

    void EntityNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        Q_ASSERT(m_ptr != nullptr);
        m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool EntityNode::playerOnly() const {
        return m_playerOnly;
    }

    void EntityNode::setPlayerOnly(bool playerOnly) {
        m_playerOnly = playerOnly;
    }

    NodePtr EntityNode::getNode() const {
        return m_ptr;
    }

    void EntityNode::setNode(const NodePtr &ptr) {
        m_ptr = ptr;
    }

    EntityNode::EntityNode(ParserType parserType, int length,
                           const NodePtr &ptr)
        : ArgumentNode(parserType, length), m_ptr(ptr) {
    }

    bool EntityNode::singleOnly() const {
        return m_singleOnly;
    }

    void EntityNode::setSingleOnly(bool singleOnly) {
        m_singleOnly = singleOnly;
    }

    static bool _2 = TypeRegister<GameProfileNode>::init();

    GameProfileNode::GameProfileNode(int length)
        : EntityNode(ParserType::GameProfile, length, nullptr) {
    }

    GameProfileNode::GameProfileNode(EntityNode *other)
        : EntityNode(ParserType::GameProfile, other->length(),
                     other->getNode()) {
    }

    void GameProfileNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        Q_ASSERT(m_ptr != nullptr);
        m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    static bool _3 = TypeRegister<ScoreHolderNode>::init();

    ScoreHolderNode::ScoreHolderNode(int length)
        : EntityNode(ParserType::ScoreHolder, length, nullptr) {
    }

    ScoreHolderNode::ScoreHolderNode(EntityNode *other)
        : EntityNode(ParserType::GameProfile, other->length(),
                     other->getNode()) {
        setSingleOnly(other->singleOnly());
        setPlayerOnly(other->playerOnly());
    }

    void ScoreHolderNode::accept(NodeVisitor *visitor, VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (!m_all) {
            Q_ASSERT(m_ptr != nullptr);
            m_ptr->accept(visitor, order);
        }
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool ScoreHolderNode::isAll() const {
        return m_all;
    }

    void ScoreHolderNode::setAll(bool all) {
        m_all = all;
    }

    static const int _EntityArgumentValueNode =
        qRegisterMetaType<QSharedPointer<EntityArgumentValueNode> >();

    EntityArgumentValueNode::EntityArgumentValueNode(
        QSharedPointer<ArgumentNode> valNode, bool negative)
        : ParseNode(Kind::Container, valNode->length()),
        m_ptr(valNode), m_negative(negative) {
    }

    EntityArgumentValueNode::EntityArgumentValueNode(bool negative)
        : ParseNode(Kind::Container, 0), m_negative(negative) {
    }

    bool EntityArgumentValueNode::isValid() const {
        return ParseNode::isValid() && m_ptr;
    }

    void EntityArgumentValueNode::accept(NodeVisitor *visitor,
                                         VisitOrder order) {
        if (order == VisitOrder::LetTheVisitorDecide) {
            visitor->visit(this);
            return;
        }
        if (order == VisitOrder::Preorder)
            visitor->visit(this);
        if (m_ptr)
            m_ptr->accept(visitor, order);
        if (order == VisitOrder::Postorder)
            visitor->visit(this);
    }

    bool EntityArgumentValueNode::isNegative() const {
        return m_negative;
    }

    void EntityArgumentValueNode::setNegative(bool negative) {
        m_negative = negative;
    }

    QSharedPointer<ArgumentNode> EntityArgumentValueNode::getNode()  const {
        return m_ptr;
    }

    void EntityArgumentValueNode::setNode(QSharedPointer<ArgumentNode> value) {
        m_ptr = value;
    }
}
