#include "listnode.h"
#include "../visitors/nodevisitor.h"

#define DEFINE_LIST_ACCEPT_METHOD(Class)                              \
        void Class::accept(NodeVisitor * visitor, VisitOrder order) { \
            if (order == VisitOrder::LetTheVisitorDecide) {           \
                visitor->visit(this);                                 \
                return;                                               \
            }                                                         \
            if (order == VisitOrder::Preorder) {                      \
                visitor->visit(this);                                 \
            }                                                         \
            for (const auto &elem: qAsConst(m_vector)) {              \
                elem->accept(visitor, order);                         \
            }                                                         \
            if (order == VisitOrder::Postorder) {                     \
                visitor->visit(this);                                 \
            }                                                         \
        }                                                             \

namespace Command {
    ListNode::ListNode(int length) : ParseNode(
            ParseNode::Kind::Container, length) {
        m_isValid = true;
    }

    DEFINE_LIST_ACCEPT_METHOD(ListNode);
}
