#include "completionprovider.h"

#include "../schema/schemanode.h"
#include "../schema/schemaargumentnode.h"
#include "../nodes/uuidnode.h"
#include "game.h"
#include "globalhelpers.h"

#include <QDebug>

namespace Command {
    CompletionProvider::CompletionProvider(const int row) : OverloadNodeVisitor(
            LetTheVisitorDecide), m_cursorRow{row} {
    }

    void CompletionProvider::visit(RootNode *node) {
        m_pos += node->trailingTrivia().length();
        const auto &children = node->children();
        if (!children.empty()) {
            auto *prevChild = children.at(0).get();
            bool  isFirst   = true;
            for (const auto &child: children) {
                m_pos += child->leadingTrivia().length();
                // qDebug() << m_pos << child << child->length() <<
                //     child->schemaNode();
                if ((m_cursorRow >= m_pos) &&
                    (m_cursorRow <= (m_pos + child->length()))) {
                    //qDebug() << "I choose you" << child;
                    if (!isFirst) {
                        Q_ASSERT(prevChild->schemaNode() != nullptr);
                        auto &&literals =
                            prevChild->schemaNode()->literalChildren();
                        if (literals.isEmpty() &&
                            prevChild->schemaNode()->redirect()) {
                            literals =
                                prevChild->schemaNode()->redirect()->
                                literalChildren();
                        }
                        m_suggestions += literals.keys().toVector();
                    } else if (child->schemaNode()) {
                        m_suggestions +=
                            child->schemaNode()->parent()->literalChildren().
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
                isFirst   = false;
            }
        }
    }

    void CompletionProvider::visit(BoolNode *) {
        m_suggestions += toStringVec(staticSuggestions<BoolNode>);
    }

    void CompletionProvider::visit(ColorNode *) {
        m_suggestions += toStringVec(staticSuggestions<ColorNode>);
    }

    void CompletionProvider::visit(EntityAnchorNode *) {
        m_suggestions += toStringVec(staticSuggestions<EntityAnchorNode>);
    }

    void CompletionProvider::visit(GamemodeNode *) {
        m_suggestions += toStringVec(staticSuggestions<GamemodeNode>);
    }

    void CompletionProvider::visit(HeightmapNode *) {
        m_suggestions += toStringVec(staticSuggestions<HeightmapNode>);
    }

    void CompletionProvider::visit(OperationNode *) {
        m_suggestions += toStringVec(staticSuggestions<OperationNode>);
    }

    void CompletionProvider::visit(ScoreboardSlotNode *) {
        if (Game::version() >= Game::v1_20_2) {
            m_suggestions += toStringVec(
                staticSuggestions_ScoreboardSlotNode_v1_20_2);
        } else {
            m_suggestions += toStringVec(staticSuggestions<ScoreboardSlotNode>);
        }
    }

    void CompletionProvider::visit(TemplateMirrorNode *) {
        m_suggestions += toStringVec(staticSuggestions<TemplateMirrorNode>);
    }

    void CompletionProvider::visit(TemplateRotationNode *) {
        m_suggestions += toStringVec(staticSuggestions<TemplateRotationNode>);
    }

    void CompletionProvider::visit(DimensionNode *) {
        addSuggestionsFromInfo(QStringLiteral("dimension"));
    }

    void CompletionProvider::visit(EntitySummonNode *) {
        addSuggestionsFromInfo(QStringLiteral("entity"));
    }

    void CompletionProvider::visit(FunctionNode *) {
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("functions"));
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("tags/functions"),
            QString(), false);
    }

    void CompletionProvider::visit(ItemEnchantmentNode *) {
        addSuggestionsFromInfo(QStringLiteral("enchantment"));
    }

    void CompletionProvider::visit(MobEffectNode *) {
        addSuggestionsFromInfo(QStringLiteral("effect"));
    }

    void CompletionProvider::visit(ResourceLocationNode *node) {
        const bool from_1_21 = Game::version() >= Game::v1_21;

        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("advancements"),
            QString(), false, from_1_21);
        m_suggestions += Game::getRegistry(QStringLiteral("advancement"));
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("item_modifiers"),
            QString(), false, from_1_21);
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("loot_tables"),
            QString(), false, from_1_21);
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("predicates"),
            QString(), false, from_1_21);
        m_suggestions += Game::getRegistry(QStringLiteral("loot_table"));
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("recipes"),
            QString(), false, from_1_21);
        m_suggestions += Game::getRegistry(QStringLiteral("recipe"));
        m_suggestions += Game::getRegistry(QStringLiteral("sound_event"));
    }

    void CompletionProvider::visit(ResourceNode *node) {
        addSuggestionsFromRegistry(node);
    }

    void CompletionProvider::visit(ResourceKeyNode *node) {
        addSuggestionsFromRegistry(node);
    }

    void CompletionProvider::visit(ResourceOrTagNode *node) {
        addSuggestionsFromRegistry(node, true);
    }

    void CompletionProvider::visit(ResourceOrTagKeyNode *node) {
        addSuggestionsFromRegistry(node, true);
    }

    void CompletionProvider::visit(BlockStateNode *node) {
        if ((m_cursorRow >= m_pos) &&
            (m_cursorRow <= (m_pos + node->resLoc()->length()))) {
            addSuggestionsFromInfo(QStringLiteral("block"));
        }
    }

    void CompletionProvider::visit(BlockPredicateNode *node) {
        if ((m_cursorRow >= m_pos) &&
            (m_cursorRow <= (m_pos + node->resLoc()->length()))) {
            addSuggestionsFromInfo(QStringLiteral("block"));
            addSuggestionsFromInfo(QStringLiteral("tag/block"), true);
            m_suggestions += Glhp::fileIdList(
                QDir::currentPath(), QStringLiteral("tags/blocks"),
                QString(), false);
        }
    }

    void CompletionProvider::visit(ItemStackNode *node) {
        if ((m_cursorRow >= m_pos) &&
            (m_cursorRow <= (m_pos + node->resLoc()->length()))) {
            addItemInfo();
        }
    }

    void CompletionProvider::visit(ItemPredicateNode *node) {
        if (node->resLoc()) {
            if ((m_cursorRow >= m_pos) &&
                (m_cursorRow <= (m_pos + node->resLoc()->length()))) {
                addItemInfo();
                addSuggestionsFromInfo(QStringLiteral("tag/item"), true);
                m_suggestions += Glhp::fileIdList(
                    QDir::currentPath(), QStringLiteral("tags/items"),
                    QString(), false);
            }
        }
    }

    void CompletionProvider::visit(ParticleNode *node) {
        if ((m_cursorRow >= m_pos) &&
            (m_cursorRow <= (m_pos + node->resLoc()->length()))) {
            m_suggestions += Game::getRegistry(QStringLiteral("particle_type"));
        }
    }

    void CompletionProvider::visit(UuidNode *) {
        m_suggestions += QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    void CompletionProvider::visit(LootModifierNode *) {
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("item_modifiers"),
            QString(), false);
        m_suggestions += Game::getRegistry(QStringLiteral("item_modifier"));
    }

    void CompletionProvider::visit(LootPredicateNode *) {
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("predicates"),
            QString(), false);
        m_suggestions += Game::getRegistry(QStringLiteral("predicate"));
    }

    void CompletionProvider::visit(LootTableNode *) {
        m_suggestions += Glhp::fileIdList(
            QDir::currentPath(), QStringLiteral("loot_tables"),
            QString(), false);
        m_suggestions += Game::getRegistry(QStringLiteral("loot_table"));
    }

    QVector<QString> CompletionProvider::suggestions() const {
        return m_suggestions;
    }

    void CompletionProvider::addSuggestionsFromRegistry(ArgumentNode *node,
                                                        const bool getTag) {
        if (node->schemaNode()->kind() == Schema::Node::Kind::Argument) {
            const auto *schemaNode =
                static_cast<const Schema::ArgumentNode *>(node->schemaNode());
            const auto &props    = schemaNode->properties();
            QString   &&registry = props.value("registry").toString();
            Glhp::removePrefix(registry, QLatin1String("minecraft:"));

            if (!registry.isEmpty()) {
                m_suggestions += Game::getRegistry(registry);
                if (getTag) {
                    registry += "tag/"_QL1;
                    const auto &&tags = Game::getRegistry(registry);
                    std::transform(tags.cbegin(), tags.cend(),
                                   std::back_inserter(m_suggestions),
                                   [](auto &&str) {
                        const static QChar hashtag = '#';
                        return hashtag + str;
                    });
                }
            }
        }
    }

    void CompletionProvider::addSuggestionsFromInfo(const QString &key,
                                                    const bool &useTagForm) {
        const QVariantMap &&infoMap = Game::getInfo(key);
        const auto        &&keys    = infoMap.keys();

        if (useTagForm) {
            std::transform(keys.cbegin(), keys.cend(),
                           std::back_inserter(m_suggestions),
                           [](auto &&str) {
                const static QChar hashtag = '#';
                return hashtag + str;
            });
        } else {
            m_suggestions += keys.toVector();
        }
    }
    void CompletionProvider::addItemInfo()
    {
        const QVariantMap&& itemInfoMap = Game::getInfo(QStringLiteral("item"));
        const auto&& keys = itemInfoMap.keys();
        m_suggestions += keys.toVector();

        const QVariantMap&& blockInfoMap = Game::getInfo(QStringLiteral("block"));
        for (auto it = blockInfoMap.cbegin(); it != blockInfoMap.cend(); ++it) {
            const auto &value = it.value();
            if ((value.type() != QVariant::Map) || !value.toMap().value("unobtainable", false).toBool()) {
                m_suggestions += it.key();
            }
        }
    }
}
