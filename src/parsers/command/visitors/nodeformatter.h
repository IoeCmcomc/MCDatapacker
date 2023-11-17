#ifndef NODEFORMATTER_H
#define NODEFORMATTER_H

#include "overloadnodevisitor.h"

#include "../nodes/rootnode.h"
#include "../nodes/literalnode.h"
#include "../nodes/stringnode.h"
#include "../nodes/axesnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/entitynode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/targetselectornode.h"
#include "codepalette.h"

#include <QTextCharFormat>
#include <QTextLayout>

namespace Command {
    using FormatRange  = QTextLayout::FormatRange;
    using FormatRanges = QVector<FormatRange>;

    class NodeFormatter : public OverloadNodeVisitor {
public:
        explicit NodeFormatter(const CodePalette &palette);

        void visit(ParseNode *node) final {
            m_pos += node->leadingTrivia().length();

            const auto &fmt = m_palette[node->kind()];
            if (fmt.propertyCount() > 0) {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               std::move(fmt) };
            } else {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               defaultFormat(node) };
            }

            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(ArgumentNode *node) final {
            m_pos += node->leadingTrivia().length();

            const auto &fmt = m_palette[node->parserType()];
            if (fmt.propertyCount() > 0) {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               std::move(fmt) };
            } else {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               defaultFormat(node) };
            }

            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(ErrorNode *node) final {
            m_pos += node->leadingTrivia().length();
            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(RootNode *node) final {
            m_pos += node->leadingTrivia().length();

            for (const auto &child: node->children()) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        void visit(BoolNode *node) final {
            m_pos += node->leadingTrivia().length();

            const FormatRange range{
                m_pos, node->length(), (node->value())
                                           ? m_palette[CodePalette::Bool_True]
                                           : m_palette[CodePalette::Bool_False]
            };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(LiteralNode *node) final {
            m_pos += node->leadingTrivia().length();

            const FormatRange range{
                m_pos, node->length(), node->isCommand()
                    ? m_palette[CodePalette::CommandLiteral]
                    : m_palette[CodePalette::CmdLiteral]
            };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(BlockStateNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->resLoc()->length(),
                                           m_palette[CodePalette::BlockState] };
            m_pos += node->resLoc()->length();

            if (node->states()) {
                m_formatRanges << FormatRange{
                    m_pos, node->states()->length(),
                    m_palette[CodePalette::BlockState_States]
                    };
                node->states()->accept(this, m_order);
            }
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }

        void visit(EntityNode *node) final {
            m_pos += node->leadingTrivia().length();
            node->getNode()->accept(this, m_order);
            m_pos += node->trailingTrivia().length();
        }

        void visit(ScoreHolderNode *node) final {
            m_pos += node->leadingTrivia().length();
            if (!node->isAll()) {
                node->getNode()->accept(this, m_order);
            } else {
                m_pos += node->length();
            }
            m_pos += node->trailingTrivia().length();
        }

        void visit(ItemStackNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->resLoc()->length(),
                                           m_palette[CodePalette::ItemStack] };
            m_pos += node->resLoc()->length();

            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        void visit(MapNode *node) final {
            m_pos += node->leadingTrivia().length() + node->leftText().length();

            const auto &pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                i->get()->first->accept(this, m_order);
                i->get()->second->accept(this, m_order);
                m_pos += i->get()->trailingTrivia().length();
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }

        void visit(EntityArgumentValueNode *node) final {
            m_pos += node->leadingTrivia().length();
            m_pos += node->leftText().length();
            node->getNode()->accept(this, m_order);
            m_pos += node->trailingTrivia().length();
        }

        void visit(NbtNode *node) final {
            m_pos += node->leadingTrivia().length();

            const auto &fmt = m_palette[node->tagType()];
            if (fmt.propertyCount() > 0) {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               std::move(fmt) };
            } else {
                m_formatRanges << FormatRange{ m_pos, node->length(),
                                               m_palette[CodePalette::NbtTag] };
            }

            m_pos += node->length() + node->trailingTrivia().length();
        }

        void visit(NbtByteArrayNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtByteArray] };
            m_pos += node->leftText().length();

            for (const auto &elem: qAsConst(node->children())) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        void visit(NbtCompoundNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtCompound] };

            m_pos += node->leftText().length();

            const auto &pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                i->get()->first->accept(this, m_order);
                i->get()->second->accept(this, m_order);
                m_pos += i->get()->trailingTrivia().length();
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        void visit(NbtIntArrayNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtIntArray] };
            m_pos += node->leftText().length();

            for (const auto &elem: qAsConst(node->children())) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        void visit(NbtListNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtList] };

            m_pos += node->leftText().length();

            for (const auto &elem: qAsConst(node->children())) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        void visit(NbtLongArrayNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtLongArray] };

            m_pos += node->leftText().length();

            for (const auto &elem: qAsConst(node->children())) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }

        void visit(NbtPathNode *node) final {
            m_pos += node->leadingTrivia().length();

            const auto &&steps = node->steps();
            for (const auto &child: steps) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        void visit(NbtPathStepNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::NbtPath_Step] };

            using Type = NbtPathStepNode::Type;
            switch (node->type()) {
                case Type::Root: {
                    node->filter()->accept(this, m_order);
                    break;
                }

                case Type::Key: {
                    node->name()->accept(this, m_order);

                    if (node->filter())
                        node->filter()->accept(this, m_order);
                    break;
                }

                case Type::Index: {
                    m_pos += node->leftText().length();
                    if (node->filter())
                        node->filter()->accept(this, m_order);
                    else if (node->index())
                        node->index()->accept(this, m_order);
                    m_pos += node->rightText().length();
                }
            }

            m_pos += node->trailingTrivia().length();
        }

        void visit(BlockPredicateNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{
                m_pos, node->resLoc()->length(),
                m_palette[CodePalette::BlockPredicate] };
            m_pos += node->resLoc()->length();

            if (node->states()) {
                m_formatRanges << FormatRange{
                    m_pos, node->states()->length(),
                    m_palette[CodePalette::BlockState_States]
                    };
                node->states()->accept(this, m_order);
            }
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }

        void visit(TargetSelectorNode *node) final {
            m_pos += node->leadingTrivia().length();

            /* Only colorize the '@x' part */
            m_formatRanges << FormatRange{
                m_pos, node->leftText().length(),
                m_palette[CodePalette::TargetSelector_Variable] };

            m_pos += node->leftText().length();

            if (node->args()) {
                m_formatRanges << FormatRange{
                    m_pos, node->args()->length(),
                    m_palette[CodePalette::TargetSelector_Arguments]
                    };
                node->args()->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }

        void visit(ParticleNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->resLoc()->length(),
                                           m_palette[CodePalette::Particle] };

            m_pos += node->resLoc()->length();
            m_pos += node->resLoc()->trailingTrivia().length();
            const auto &&params = node->params();
            if (!params.isEmpty()) {
                for (const auto &child: params) {
                    child->accept(this, m_order);
                }
            }

            m_pos += node->trailingTrivia().length();
        }
        void visit(ParticleColorNode *node) final {
            m_pos += node->leadingTrivia().length();

            node->r()->accept(this, m_order);
            node->g()->accept(this, m_order);
            node->b()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        void visit(ItemPredicateNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->resLoc()->length(),
                                           m_palette[CodePalette::ItemPredicate] };

            m_pos += node->resLoc()->length();
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }

        void visit(KeyNode *node) final {
            m_pos += node->leadingTrivia().length();

            m_formatRanges << FormatRange{ m_pos, node->length(),
                                           m_palette[CodePalette::Key] };

            m_pos += node->length() + node->trailingTrivia().length();
        }

        FormatRanges formatRanges() const;
        void reset();

private:
        FormatRanges m_formatRanges;
        CodePalette m_palette;
        int m_pos = 0;

        QTextCharFormat defaultFormat([[maybe_unused]] ParseNode *node) const;
    };
}

#endif /* NODEFORMATTER_H */
