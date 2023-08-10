#include "completionprovider.h"

#include "../schema/schemanode.h"
#include "../schema/schemaargumentnode.h"
#include "game.h"
#include "globalhelpers.h"

namespace Command {
    CompletionProvider::CompletionProvider(const int row) : OverloadNodeVisitor(
            LetTheVisitorDecide), m_cursorRow{row} {
    }

    void CompletionProvider::visit(RootNode *node) {
        m_pos += node->trailingTrivia().length();
        const auto &children = node->children();
        if (!children.empty()) {
            auto *prevChild = children.at(0).get();
            for (const auto &child: children) {
                m_pos += child->leadingTrivia().length();
                qDebug() << m_pos << child->length() << child->schemaNode();
                if ((m_cursorRow >= m_pos) && (m_cursorRow <= (m_pos + child->length()))) {
                    if (children.size() > 1) {
                        Q_ASSERT(prevChild->schemaNode() != nullptr);
                        m_suggestions += prevChild->schemaNode()->literalChildren().
                                         keys().toVector();
                    } else {
                        Q_ASSERT(child->schemaNode() != nullptr);
                        m_suggestions += child->schemaNode()->parent()->literalChildren().
                                         keys().toVector();
                    }
                    child->accept(this, m_order);

                    break;
                }
                m_pos += child->leftText().length();
                m_pos += child->length();
                m_pos += child->rightText().length() +
                         child->trailingTrivia().length();
                prevChild = child.get();
            }
        }
    }

    void CompletionProvider::visit(GamemodeNode *node) {
        m_suggestions +=
        { "adventure", "creative", "survival", "spectator" };
    }

    void CompletionProvider::visit(ResourceNode *node) {
        if (node->schemaNode()->kind() == Schema::Node::Kind::Argument) {
            const auto *schemaNode =
                static_cast<const Schema::ArgumentNode *>(node->schemaNode());
            const auto  &props    = schemaNode->properties();
            QString &&registry = props.value("registry").toString();
            Glhp::removePrefix(registry, QLatin1String("minecraft:"));

            if (!registry.isEmpty()) {
                m_suggestions += Game::getRegistry(registry);
            }
        }
    }

    QVector<QString> CompletionProvider::suggestions() const {
        return m_suggestions;
    }
}
