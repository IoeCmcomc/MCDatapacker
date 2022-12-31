#ifndef COMPONENTNODE_H
#define COMPONENTNODE_H

#include "argumentnode.h"

#include "nlohmann/json.hpp"

namespace Command {
    class ComponentNode final : public ArgumentNode
    {
public:
        explicit ComponentNode(const QString &text);

        bool isValid() const override;
        void accept(NodeVisitor *visitor, VisitOrder)  override;

        nlohmann::json value() const;
        void setValue(const nlohmann::json &value);

private:
        nlohmann::json m_value;
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Component)
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ComponentNode>)

#endif /* COMPONENTNODE_H */
