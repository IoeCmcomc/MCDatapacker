#ifndef NODEFORMATTER_H
#define NODEFORMATTER_H

#include "nodevisitor.h"

#include "../nodes/rootnode.h"
#include "../nodes/boolnode.h"
#include "../nodes/doublenode.h"
#include "../nodes/floatnode.h"
#include "../nodes/integernode.h"
#include "../nodes/stringnode.h"
#include "../nodes/rootnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/componentnode.h"
#include "../nodes/entitynode.h"
#include "../nodes/floatrangenode.h"
#include "../nodes/intrangenode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/multimapnode.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/similaraxesnodes.h"
#include "../nodes/similarresourcelocationnodes.h"
#include "../nodes/similarstringnodes.h"
#include "../nodes/swizzlenode.h"
#include "../nodes/timenode.h"

#include <QRandomGenerator>
#include <QTextCharFormat>
#include <QTextLayout>

namespace Command {
    class NodeFormatter : public NodeVisitor
    {
public:
        NodeFormatter();

        virtual void visit(ParseNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(RootNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(ArgumentNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(BoolNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setFontWeight(QFont::DemiBold);
            fmt.setForeground(
                (node->value()) ? QColor(0, 190, 0) : QColor(190, 0, 0));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(DoubleNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97C1"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(FloatNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97B1"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(IntegerNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#2F97A1"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(StringNode *node) {
            /*auto fmt = defaultFormat(node); */

            if (node->length() - node->value().length() != 2) {
                QTextCharFormat fmt;
                fmt.setForeground(QColor("#a33016"));
                QTextLayout::FormatRange range{ node->pos(), node->length(),
                                                std::move(fmt) };

                m_formatRanges << std::move(range);
            }
        }
        virtual void visit(LiteralNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(Qt::blue);
            fmt.setFontWeight(QFont::Bold);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(AxesNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#AB81CD"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(BlockStateNode *node) {
            const int length = node->length();

            node->setLength(node->format().length());
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#7d7b55"));

            QTextLayout::FormatRange range{ node->pos(), length,
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
            node->setLength(length);
        }
        virtual void visit(ComponentNode *node) {
            /*auto fmt = defaultFormat(node); */

            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaffe8f2"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(EntityNode *node) {
            auto fmt = defaultFormat(node);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }

        virtual void visit(GameProfileNode *node) {
            auto fmt = defaultFormat(node);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(ScoreHolderNode *node) {
            auto fmt = defaultFormat(node);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(FloatRangeNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#1e9cb0"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(IntRangeNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#1e9cb0"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ItemStackNode *node) {
            const int length = node->length();

            node->setLength(node->format().length());
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#7d7b55"));

            QTextLayout::FormatRange range{ node->pos(), length,
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
            node->setLength(length);
        }
        virtual void visit(MapNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aafaf0f5"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(MultiMapNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaebfcec"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(EntityArgumentValueNode *node) {
            auto fmt = defaultFormat(node);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(NbtNode *node) {
            auto fmt = defaultFormat(node);

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(NbtByteArrayNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtByteNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtCompoundNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aae8e9ff"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtDoubleNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtFloatNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtIntArrayNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtIntNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtListNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtLongArrayNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setBackground(QColor("#aaf1edfa"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtLongNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtShortNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#226467"));

            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(NbtStringNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(NbtPathNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(NbtPathStepNode *node) {
            using Type = Command::NbtPathStepNode::Type;
            switch (node->type()) {
            case Type::Root: {
                if (const auto filter = node->filter())
                    filter->accept(this);
                break;
            }

            case Type::Key: {
                node->name()->accept(this);
                if (const auto filter = node->filter())
                    filter->accept(this);
                break;
            }

            case Type::Index: {
                if (const auto filter = node->filter())
                    filter->accept(this);
                if (const auto index = node->index())
                    index->accept(this);
                break;
            }
            }

            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setBackground(QColor("#aafff4e8"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ResourceLocationNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#45503B"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(BlockPosNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(BlockPredicateNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#7d7b55"));
            fmt.setFontItalic(true);

            QTextLayout::FormatRange range{ node->pos(),
                                     node->format().length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ColumnPosNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(RotationNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(Vec2Node *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(Vec3Node *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(DimensionNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(EntitySummonNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(FunctionNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ItemEnchantmentNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(MobEffectNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ColorNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(EntityAnchorNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ItemSlotNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(MessageNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(ObjectiveNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ObjectiveCriteriaNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(OperationNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ScoreboardSlotNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(TeamNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(SwizzleNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#de59ce"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(TargetSelectorNode *node) {
            const int length = node->length();

            node->setLength(2);
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#276321"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
            node->setLength(length);
        }
        virtual void visit(TimeNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#5C37A5"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(UuidNode *node) {
            /*auto                     fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#9c442f"));
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(ParticleNode *node) {
            const int length = node->length();

            node->setLength(node->format().length());
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;
            fmt.setForeground(QColor("#49B13B"));

            QTextLayout::FormatRange range{ node->pos(), length,
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
            node->setLength(length);
        }
        virtual void visit(ParticleColorNode *node) {
            auto                     fmt = defaultFormat(node);
            QTextLayout::FormatRange range{ node->pos(), node->length(),
                                            std::move(fmt) };

            /*m_formatRanges << std::move(range); */
        }
        virtual void visit(ItemPredicateNode *node) {
            /*auto fmt = defaultFormat(node); */
            QTextCharFormat fmt;

            fmt.setForeground(QColor("#B89C3D"));
            fmt.setFontItalic(true);

            QTextLayout::FormatRange range{ node->pos(),
                                     node->format().length(),
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }
        virtual void visit(const MapKey &key) {
            /*auto fmt    = m_bgfmt; */
            int length = key.text.length() + (key.isQuote * 2);

            QTextCharFormat fmt;

            fmt.setForeground(QColor("#B89C3D"));

            fmt.setToolTip(QString("Pos: %1; Length: %2; Index: %3").arg(
                               key.pos).arg(length).arg(
                               m_formatRanges.length()));

            QTextLayout::FormatRange range{ key.pos, length,
                                            std::move(fmt) };

            m_formatRanges << std::move(range);
        }

        QVector<QTextLayout::FormatRange> formatRanges() const;

private:
        QTextCharFormat m_bgfmt;
        QVector<QTextLayout::FormatRange> m_formatRanges;
        QTextCharFormat defaultFormat(ParseNode *node) const {
            auto fmt = m_bgfmt;

            fmt.setBackground(QColor(QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     QRandomGenerator::global()->bounded(127,
                                                                         255),
                                     220));

            fmt.setToolTip(QString("Pos: %1; Length: %2; Index: %3; %4").arg(
                               node->pos()).arg(node->length()).arg(
                               m_formatRanges.length()).arg(node->toString()));
            return fmt;
        }
        virtual void visit(AxisNode *node) {
            Q_UNUSED(node)
        };
    };
}

#endif /* NODEFORMATTER_H */
