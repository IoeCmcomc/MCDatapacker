#ifndef COMPONENTNODE_H
#define COMPONENTNODE_H

#include "argumentnode.h"

#include "nlohmann/json.hpp"

namespace Command {
    class ComponentNode final : public ArgumentNode {
public:
        explicit ComponentNode(const QString &text);

        void accept(NodeVisitor *visitor, VisitOrder) final;

        nlohmann::json value() const;
        void setValue(nlohmann::json value);

private:
        nlohmann::json m_value;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Component)
}
#endif /* COMPONENTNODE_H */
