#ifndef SCHEMA_NODE_H
#define SCHEMA_NODE_H

#include "nlohmann/json.hpp"

#include <QMap>
#include <QHash>

using json = nlohmann::json;

// Source: https://github.com/nlohmann/json/issues/274#issuecomment-305324120
inline void to_json(json& j, const QString& q) {
    j = json(q.toStdString());
}

inline void from_json(const json& j, QString& q) {
    q = QString::fromStdString(j.get<std::string>());
}

namespace Command::Schema {
    class LiteralNode;
    class ArgumentNode;

    class Node {
public:
        enum class Kind: uint16_t {
            Root,
            Literal,
            Argument,
            Unknown
        };
        using LiteralChildrenType = QMap<QString, LiteralNode *>;

        Node();
        ~Node();

        Kind kind() const;

        LiteralChildrenType literalChildren() const;
        QVector<ArgumentNode *> argumentChildren() const;

        bool inline isEmpty() const {
            return m_literalChildren.empty() && m_argumentChildren.isEmpty();
        }
        bool isExecutable() const;
        Node * redirect() const;
        void setRedirect(Node *newRedirect);

protected:
        LiteralChildrenType m_literalChildren;
        QVector<ArgumentNode *> m_argumentChildren;
        Node *m_redirect  = nullptr;
        Kind m_kind       = Kind::Unknown;
        bool m_executable = false;

        explicit Node(Kind kind);

        void _from_json(const json &j);
    };
}

#endif // SCHEMA_NODE_H
