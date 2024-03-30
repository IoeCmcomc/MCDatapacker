#include "schemanode.h"

#include "schemaargumentnode.h"
#include "schemaliteralnode.h"

#include "nlohmann/json.hpp"

namespace Command::Schema {
    Node::Node() {
    }

    Node::~Node() {
        qDeleteAll(m_literalChildren);
        qDeleteAll(m_argumentChildren);
    }

    Node::Kind Node::kind() const {
        return m_kind;
    }

    Node::LiteralChildrenType Node::literalChildren() const {
        return m_literalChildren;
    }

    QVector<ArgumentNode *> Node::argumentChildren() const {
        return m_argumentChildren;
    }

    bool Node::isExecutable() const {
        return m_executable;
    }

    Node * Node::redirect() const {
        return m_redirect;
    }

    void Node::setRedirect(Node *newRedirect) {
        m_redirect = newRedirect;
    }

    Node * Node::parent() const {
        return m_parent;
    }

    Node::Node(Kind kind) : m_kind(kind) {
    }

    void Node::_from_json(const json &j) {
        if (j.contains("executable")) {
            j.at("executable").get_to(m_executable);
        }

        if (!j.contains("children")) {
            return;
        }

        for (auto& [key, val] : j.at("children").items()) {
            if (val["type"] == "literal") {
                LiteralNode *child = val.get<LiteralNode *>();
                child->m_parent                                = this;
                m_literalChildren[QString::fromStdString(key)] = child;
            } else if (val.at("type") == "argument") {
                ArgumentNode *child = val.get<ArgumentNode *>();
                child->m_parent = this;
                child->setName(QString::fromStdString(key));
                m_argumentChildren << child;
            }
        }
    }
}
