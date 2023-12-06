#ifndef STYLENODE_H
#define STYLENODE_H

#include "argumentnode.h"

#include "nlohmann/json.hpp"

namespace Command {
    class StyleNode final : public ArgumentNode {
public:
        explicit StyleNode(const QString &text);

        void accept(NodeVisitor *visitor, VisitOrder) final;

        nlohmann::json value() const;
        void setValue(nlohmann::json value);

private:
        nlohmann::json m_value;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Style)
}
#endif /* STYLENODE_H */
