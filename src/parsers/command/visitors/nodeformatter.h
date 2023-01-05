#ifndef NODEFORMATTER_H
#define NODEFORMATTER_H

#include "overloadnodevisitor.h"

#include "../nodes/rootnode.h"
#include "../nodes/literalnode.h"
#include "../nodes/stringnode.h"
#include "../nodes/axesnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/componentnode.h"
#include "../nodes/entitynode.h"
#include "../nodes/floatrangenode.h"
#include "../nodes/intrangenode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/swizzlenode.h"
#include "../nodes/timenode.h"
#include "../nodes/targetselectornode.h"

#include <QRandomGenerator>
#include <QTextCharFormat>
#include <QTextLayout>

namespace Command {
    class NodeFormatter : public OverloadNodeVisitor
    {
public:
        NodeFormatter();

        virtual void visit(ParseNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(RootNode *node) override {
            m_pos += node->leadingTrivia().length();

            for (const auto &child: node->children()) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(BoolNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setFontWeight(QFont::DemiBold);
            fmt.setForeground(
                (node->value()) ? QColor(0, 190, 0) : QColor(190, 0, 0));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(DoubleNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97C1"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(FloatNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97B1"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(IntegerNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97A1"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(StringNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;
            fmt.setForeground(QColor("#a33016"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(LiteralNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(Qt::blue);
            fmt.setFontWeight(QFont::Bold);

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(AngleNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#7404cf"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(BlockStateNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;
            fmt.setForeground(QColor("#7d7b55"));

            QTextLayout::FormatRange range{ m_pos, node->resLoc()->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->resLoc()->length();
            if (node->states())
                node->states()->accept(this, m_order);
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(ComponentNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaffe8f2"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(EntityNode *node) override {
            m_pos += node->leadingTrivia().length();
            node->getNode()->accept(this, m_order);
            m_pos += node->trailingTrivia().length();
        }

        virtual void visit(GameProfileNode *node) override {
            m_pos += node->leadingTrivia().length();
            node->getNode()->accept(this, m_order);
            m_pos += node->trailingTrivia().length();
        }

        virtual void visit(ScoreHolderNode *node) override {
            m_pos += node->leadingTrivia().length();
            if (!node->isAll()) {
                node->getNode()->accept(this, m_order);
            } else {
                m_pos += node->length();
            }
            m_pos += node->trailingTrivia().length();
        }
        virtual void visit([[maybe_unused]] FloatRangeNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#1e9cb0"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(IntRangeNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#1e9cb0"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ItemStackNode *node) override {
            m_pos += node->leadingTrivia().length();

            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#7d7b55"));

            QTextLayout::FormatRange range{ m_pos, node->resLoc()->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->resLoc()->length();
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(MapNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aafaf0f5"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

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

        virtual void visit(EntityArgumentValueNode *node) override {
            m_pos += node->leadingTrivia().length();
            m_pos += node->leftText().length();
            node->getNode()->accept(this, m_order);
            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(NbtNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(NbtByteArrayNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->leftText().length();

            for (const auto &child: node->children()) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(NbtCompoundNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aae8e9ff"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

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
        virtual void visit(NbtIntArrayNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            for (const auto &child: node->children()) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(NbtListNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->leftText().length();

            for (const auto &elem: node->children()) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        virtual void visit(NbtLongArrayNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->leftText().length();

            for (const auto &elem: node->children()) {
                elem->accept(this, m_order);
            }

            m_pos += node->rightText().length() +
                     node->trailingTrivia().length();
        }
        //        virtual void visit(NbtStringNode *node) override {
        //            m_pos += node->leadingTrivia().length();

        //            auto                     fmt = defaultFormat(node);
        //            QTextLayout::FormatRange range{ m_pos, node->length(),
        //                                            std::move(fmt) };

        //            /*m_formatRanges << std::move(range); */

        //            m_pos += node->length() + node->trailingTrivia().length();
        //        }
        virtual void visit(NbtPathNode *node) override {
            m_pos += node->leadingTrivia().length();

            for (const auto &child: node->steps()) {
                child->accept(this, m_order);
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(NbtPathStepNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aafff4e8"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            using Type = NbtPathStepNode::Type;
            switch (node->type()) {
                case Type::Root: {
                    node->filter()->accept(this, m_order);
                    break;
                }

                case Type::Key: {
                    return node->name()->accept(this, m_order);

                    break;
                }

                case Type::Index: {
                    if (node->filter())
                        node->filter()->accept(this, m_order);
                    else if (node->index())
                        node->index()->accept(this, m_order);
                }
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(ResourceLocationNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#45503B"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(BlockPosNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(BlockPredicateNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#7d7b55"));
            fmt.setFontItalic(true);

            QTextLayout::FormatRange range{ m_pos, node->resLoc()->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->resLoc()->length();
            if (node->states())
                node->states()->accept(this, m_order);

            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(ColumnPosNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(RotationNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(Vec2Node *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(Vec3Node *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(DimensionNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(EntitySummonNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(FunctionNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ItemEnchantmentNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(MobEffectNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ColorNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(EntityAnchorNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ItemSlotNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(MessageNode *node) override {
            m_pos += node->leadingTrivia().length();
            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ObjectiveNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ObjectiveCriteriaNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(OperationNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ScoreboardSlotNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(TeamNode *node) override {
            m_pos += node->leadingTrivia().length();

            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(SwizzleNode *node) override {
            m_pos += node->leadingTrivia().length();

            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#de59ce"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(TargetSelectorNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#276321"));
            /* Only colorize the '@x' part */
            QTextLayout::FormatRange range{ m_pos, node->leftText().length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->leftText().length();

            if (node->args())
                node->args()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(TimeNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#5C37A5"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(UuidNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#9c442f"));
            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }
        virtual void visit(ParticleNode *node) override {
            m_pos += node->leadingTrivia().length();
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#49B13B"));

            QTextLayout::FormatRange range{ m_pos, node->resLoc()->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->resLoc()->length();
            if (!node->params().isEmpty()) {
                for (const auto &child: node->params()) {
                    child->accept(this, m_order);
                }
            }

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(ParticleColorNode *node) override {
            m_pos += node->leadingTrivia().length();

            node->r()->accept(this, m_order);
            node->g()->accept(this, m_order);
            node->b()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(ItemPredicateNode *node) override {
            m_pos += node->leadingTrivia().length();

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#B89C3D"));
            fmt.setFontItalic(true);

            QTextLayout::FormatRange range{ m_pos, node->resLoc()->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->resLoc()->length();
            if (node->nbt())
                node->nbt()->accept(this, m_order);

            m_pos += node->trailingTrivia().length();
        }
        virtual void visit(KeyNode *node) override {
            m_pos += node->leadingTrivia().length();
            /*auto fmt    = m_bgfmt; */

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#B89C3D"));

            QTextLayout::FormatRange range{ m_pos, node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);

            m_pos += node->length() + node->trailingTrivia().length();
        }

        QVector<QTextLayout::FormatRange> formatRanges() const;

private:
        QTextCharFormat m_bgfmt;
        QVector<QTextLayout::FormatRange> m_formatRanges;
        int m_pos = 0;

        QTextCharFormat defaultFormat([[maybe_unused]] ParseNode *node) const {
            auto fmt = m_bgfmt;

            fmt.setBackground(QColor(QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     220));

            return fmt;
        }
    };
}

#endif /* NODEFORMATTER_H */
