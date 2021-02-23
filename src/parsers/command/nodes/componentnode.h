#ifndef COMPONENTNODE_H
#define COMPONENTNODE_H

#include "argumentnode.h"

#include "json/single_include/nlohmann/json.hpp"

#include <QJsonValue>

namespace Command {
    class ComponentNode final : public ArgumentNode
    {
        Q_OBJECT
public:
        ComponentNode(QObject *parent, int pos, int length);
        QString toString() const override;
        bool isVaild() const override;

        nlohmann::json value() const;
        void setValue(const nlohmann::json &value);

private:
        nlohmann::json m_value;
    };
}

Q_DECLARE_METATYPE(Command::ComponentNode*)

#endif /* COMPONENTNODE_H */
