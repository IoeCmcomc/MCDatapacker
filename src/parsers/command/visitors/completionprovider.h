#ifndef COMPLETIONPROVIDER_H
#define COMPLETIONPROVIDER_H

#include "overloadnodevisitor.h"

namespace Command {
    class CompletionProvider : public OverloadNodeVisitor {
public:
        explicit CompletionProvider(const int row);

        void visit(RootNode *node) final;

        void visit(BoolNode *node) final;
        void visit(ColorNode *node) final;
        void visit(EntityAnchorNode *node) final;
        void visit(GamemodeNode *node) final;
        void visit(HeightmapNode *node) final;
        void visit(OperationNode *node) final;
        void visit(ScoreboardSlotNode *node) final;
        void visit(TemplateMirrorNode *node) final;
        void visit(TemplateRotationNode *node) final;
        void visit(DimensionNode *node) final;
        void visit(EntitySummonNode *node) final;
        void visit(FunctionNode *node) final;
        void visit(ItemEnchantmentNode *node) final;
        void visit(MobEffectNode *node) final;
        void visit(ResourceLocationNode *node) final;
        void visit(ResourceNode *node) final;
        void visit(ResourceKeyNode *node) final;
        void visit(ResourceOrTagNode *node) final;
        void visit(ResourceOrTagKeyNode *node) final;
        void visit(BlockStateNode *node) final;
        void visit(BlockPredicateNode *node) final;
        void visit(ItemStackNode *node) final;
        void visit(ItemPredicateNode *node) final;
        void visit(ParticleNode *node) final;
        void visit(UuidNode *node) final;
        void visit(LootModifierNode *node) final;
        void visit(LootPredicateNode *node) final;
        void visit(LootTableNode *node) final;

        QVector<QString> suggestions() const;

private:
        QVector<QString> m_suggestions;
        int m_pos       = 0;
        int m_cursorRow = 0;

        void addSuggestionsFromRegistry(ArgumentNode *node,
                                        const bool getTag = false);
        void addSuggestionsFromInfo(const QString &key,
                                    const bool &useTagForm = false);
        void addItemInfo();
    };
}

#endif // COMPLETIONPROVIDER_H
