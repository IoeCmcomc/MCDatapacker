#include "schemaargumentnode.h"

namespace Command::Schema {
    ArgumentNode::ArgumentNode() : Node(Node::Kind::Argument) {
    }

    void ArgumentNode::setName(const QString &newName) {
        m_name = newName;
    }

    QString ArgumentNode::name() const {
        return m_name;
    }

    QVariantMap ArgumentNode::properties() const {
        return m_props;
    }

    QString ArgumentNode::parserId() const {
        return m_parserId;
    }

    void from_json(const json &j, ArgumentNode *&n) {
        n = new ArgumentNode();
        j.at("parser").get_to(n->m_parserId);

        if (j.contains("properties")) {
            for (auto& [key, val] : j.at("properties").items()) {
                using Type = json::value_t;

                const QString &mapKey = QString::fromStdString(key);

                switch (val.type()) {
                    case Type::boolean: {
                        n->m_props[mapKey] = val.get<bool>();
                        break;
                    }

                    case Type::string: {
                        n->m_props[mapKey] = val.get<QString>();
                        break;
                    }

                    case Type::number_integer: {
                        n->m_props[mapKey] = val.get<int>();
                        break;
                    }

                    case Type::number_float: {
                        n->m_props[mapKey] = val.get<double>();
                        break;
                    }

                    case Type::number_unsigned: {
                        n->m_props[mapKey] = val.get<unsigned int>();
                        break;
                    }

                    case Type::null: {
                        n->m_props[mapKey] = QVariant();
                        break;
                    }

                    default:
                        n->m_props[mapKey] = QVariant();
                        break;
                }
            }
        }

        n->_from_json(j);
    }
}
