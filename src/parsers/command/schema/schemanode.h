#ifndef SCHEMA_NODE_H
#define SCHEMA_NODE_H

#include <nlohmann/json_fwd.hpp>

#include <QMap>
#include <QHash>

using json = nlohmann::json;


namespace Command::Schema {
    class LiteralNode;
    class ArgumentNode;
    class RootNode;

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

    template<class T>
    constexpr auto typeToEnum = Node::Kind::Unknown;
    template<>
    constexpr auto typeToEnum<LiteralNode> = Node::Kind::Literal;
    template<>
    constexpr auto typeToEnum<ArgumentNode> = Node::Kind::Argument;
    template<>
    constexpr auto typeToEnum<RootNode> = Node::Kind::Root;
}

template<class T>
T schemanode_cast(Command::Schema::Node *node) {
    if (node->kind() ==
        Command::Schema::typeToEnum<std::remove_pointer_t<T> >) {
        return static_cast<T>(node);
    } else {
        return nullptr;
    }
}
template<class T>
T schemanode_cast(const Command::Schema::Node *node) {
    if (node->kind() == Command::Schema::typeToEnum<
            std::remove_const_t<std::remove_pointer_t<T> > >) {
        return static_cast<T>(node);
    } else {
        return nullptr;
    }
}

#endif // SCHEMA_NODE_H
