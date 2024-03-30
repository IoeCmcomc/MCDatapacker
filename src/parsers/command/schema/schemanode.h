#ifndef SCHEMA_NODE_H
#define SCHEMA_NODE_H

#include <nlohmann/json_fwd.hpp>

#include <QMap>
#include <QHash>

using json = nlohmann::json;


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
        Node * parent() const;

protected:
        LiteralChildrenType m_literalChildren;
        QVector<ArgumentNode *> m_argumentChildren;
        Node *m_parent    = nullptr;
        Node *m_redirect  = nullptr;
        Kind m_kind       = Kind::Unknown;
        bool m_executable = false;

        explicit Node(Kind kind);

        void _from_json(const json &j);
    };
}

#endif // SCHEMA_NODE_H
