#ifndef SCHEMA_ARGUMENTNODE_H
#define SCHEMA_ARGUMENTNODE_H

#include "schemanode.h"
#include "../nodes/argumentnode.h"

#include "nlohmann/json.hpp"

#include <QVariant>

using json = nlohmann::json;

// Source: https://github.com/nlohmann/json/issues/274#issuecomment-305324120
inline void to_json(json& j, const QString& q) {
    j = json(q.toStdString());
}

inline void from_json(const json& j, QString& q) {
    q = QString::fromStdString(j.get_ref<const json::string_t &>());
}

namespace Command::Schema {
    class ArgumentNode : public Node {
public:
        using ParserType = Command::ArgumentNode::ParserType;

        ArgumentNode();

        void setName(const QString &newName);

        QString name() const;
        QVariantMap properties() const;
        void setProperties(const QVariantMap &value);
        ParserType parserType() const;


private:
        QString m_name;
        QVariantMap m_props;
        ParserType m_parserType;

        friend void from_json(const json &j, ArgumentNode *&n);
    };

    void from_json(const json &j, ArgumentNode *&n);
}

#endif // SCHEMA_ARGUMENTNODE_H
