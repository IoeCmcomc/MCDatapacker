#ifndef COMPONENTNODE_H
#define COMPONENTNODE_H

#include "argumentnode.h"

#include "nlohmann/json.hpp"

namespace Command {
    class ComponentNode final : public ArgumentNode
    {
public:
        ComponentNode(int pos, int length);
        QString toString() const override;
        bool isVaild() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }

        nlohmann::json value() const;
        void setValue(const nlohmann::json &value);

private:
        nlohmann::json m_value;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ComponentNode>)

#endif /* COMPONENTNODE_H */
