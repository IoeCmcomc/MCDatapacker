#ifndef INLINABLERESOURCENODE_H
#define INLINABLERESOURCENODE_H

#include "argumentnode.h"

namespace Command {
    class InlinableResourceNode : public ArgumentNode  {
public:
        void accept(NodeVisitor *visitor, VisitOrder order) override;

        NodePtr getNode() const;
        template <typename T>
        typename std::enable_if_t<std::is_assignable_v<NodePtr, T> >
        setNode(T &&node) {
            setLength(node->length());
            m_isValid = node->isValid();
            m_ptr     = std::forward<T>(node);
        }

protected:
        NodePtr m_ptr;

        explicit InlinableResourceNode(ParserType parserType, int length,
                                       const NodePtr &ptr = nullptr);
    };

    class LootModifierNode final : public InlinableResourceNode {
public:
        explicit LootModifierNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    class LootPredicateNode final : public InlinableResourceNode {
public:
        explicit LootPredicateNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };

    class LootTableNode final : public InlinableResourceNode {
public:
        explicit LootTableNode(int length);

        void accept(NodeVisitor *visitor, VisitOrder order) override;
    };
}

#endif // INLINABLERESOURCENODE_H
