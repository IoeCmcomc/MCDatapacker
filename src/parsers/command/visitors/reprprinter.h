#ifndef REPRPRINTER_H
#define REPRPRINTER_H

#include "nodevisitor.h"

#include "../nodes/axesnode.h"
#include "../nodes/blockstatenode.h"
#include "../nodes/componentnode.h"
#include "../nodes/entitynode.h"
#include "../nodes/floatrangenode.h"
#include "../nodes/gamemodenode.h"
#include "../nodes/inlinableresourcenode.h"
#include "../nodes/internalregexpatternnode.h"
#include "../nodes/intrangenode.h"
#include "../nodes/itemstacknode.h"
#include "../nodes/literalnode.h"
#include "../nodes/nbtnodes.h"
#include "../nodes/nbtpathnode.h"
#include "../nodes/particlenode.h"
#include "../nodes/resourcelocationnode.h"
#include "../nodes/rootnode.h"
#include "../nodes/stringnode.h"
#include "../nodes/stylenode.h"
#include "../nodes/swizzlenode.h"
#include "../nodes/targetselectornode.h"
#include "../nodes/timenode.h"
#include "../nodes/uuidnode.h"

#include <QDebug>

namespace Command {
    class ReprPrinter : public NodeVisitor
    {
public:
        explicit ReprPrinter();

        void visit(ParseNode *node) final {
            m_repr += QString("ParseNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown ParseNode detected.";
        };
        void visit(ErrorNode *node) final {
            m_repr += QString("ErrorNode[%1]").arg(node->length());
        };
        void visit(RootNode *node) final {
            m_repr += QString("RootNode[%1](").arg(node->size());
            for (const auto &child: node->children()) {
                child->accept(this, m_order);
                if (child != node->children().back()) {
                    m_repr += ", ";
                }
            }
            m_repr += ')';
        };
        void visit(ArgumentNode *node) final {
            m_repr += QString("ArgumentNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown ArgumentNode detected.";
        };
        void visit(BoolNode *node) final {
            m_repr += QString("BoolNode(%1)").arg(
                node->value() ? "true" : "false");
        };
        void visit(DoubleNode *node) final {
            m_repr += QString("DoubleNode(%1)").arg(node->value());
        };
        void visit(FloatNode *node) final {
            m_repr += QString("FloatNode(%1)").arg(node->value());
        };
        void visit(IntegerNode *node) final {
            m_repr += QString("IntegerNode(%1)").arg(node->value());
        };
        void visit(LongNode *node) final {
            m_repr += QString("LongNode(%1)").arg(node->value());
        };
        void visit(StringNode *node) final {
            m_repr += QString("StringNode(\"%1\")").arg(node->value());
        };
        void visit(LiteralNode *node) final {
            m_repr += QString("LiteralNode(%1)").arg(node->text());
        };
        void visit(AngleNode *node) final {
            m_repr += QString("AngleNode(%1)").arg(node->text());
        };
        void visit(BlockStateNode *node) final {
            m_repr += "BlockStateNode(";
            node->resLoc()->accept(this, m_order);
            m_repr += ')';
            if (node->states() && !node->states()->isEmpty()) {
                m_repr += '[';
                node->states()->accept(this, m_order);
                m_repr += ']';
            }
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        void visit(ComponentNode *node) final {
            m_repr += QString("ComponentNode(%1)").arg(
                node->value().dump().c_str());
        };
        void visit(StyleNode *node) final {
            m_repr += QString("StyleNode(%1)").arg(
                node->value().dump().c_str());
        };
        void visit(EntityNode *node) final {
            m_repr += "EntityNode";
            reprEntityNode(node);
        };
        void visit(GameProfileNode *node) final {
            m_repr += "GameProfileNode";
            reprEntityNode(node);
        };
        void visit(ScoreHolderNode *node) final {
            m_repr += "ScoreHolderNode";
            if (node->isAll()) {
                m_repr += "(*)";
            } else {
                reprEntityNode(node);
            }
        };
        void visit(FloatRangeNode *node) final {
            m_repr += QString("FloatRangeNode(%1)").arg(node->format());
        };
        void visit(IntRangeNode *node) final {
            m_repr += QString("IntRangeNode(%1)").arg(node->format());
        };
        void visit(ItemStackNode *node) final {
            m_repr += "ItemStackNode(";
            node->resLoc()->accept(this, m_order);
            m_repr += ')';
            if (node->components() && !node->components()->isEmpty()) {
                m_repr += '[';
                node->components()->accept(this, m_order);
                m_repr += ']';
            }
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        void visit(MapNode *node) final {
            m_repr += "MapNode(";

            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                i->get()->first->accept(this, m_order);
                m_repr += ": ";
                i->get()->second->accept(this, m_order);
                if ((*i) != pairs.constLast())
                    m_repr += ", ";
            }

            m_repr += ')';
        };
        void visit(EntityArgumentValueNode *node) final {
            m_repr += "EntityArgumentValueNode";
            if (node->isNegative())
                m_repr += "[!]";
            m_repr += '(';
            if (node->getNode())
                node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        void visit(NbtNode *node) final {
            m_repr += QString("NbtNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown NbtNode detected.";
        };
        void visit(NbtByteArrayNode *node) final {
            m_repr += QString("NbtByteArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        void visit(NbtByteNode *node) final {
            m_repr += QString("NbtByteNode(%1)").arg(node->value());
        };
        void visit(NbtCompoundNode *node) final {
            m_repr += "NbtCompoundNode(";

            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                i->get()->first->accept(this, m_order);
                m_repr += ": ";
                i->get()->second->accept(this, m_order);
                if ((*i) != pairs.constLast())
                    m_repr += ", ";
            }

            m_repr += ')';
        }
        void visit(NbtDoubleNode *node) final {
            m_repr += QString("NbtDoubleNode(%1)").arg(node->value());
        };
        void visit(NbtFloatNode *node) final {
            m_repr += QString("NbtFloatNode(%1)").arg(node->value());
        };
        void visit(NbtIntArrayNode *node) final {
            m_repr += QString("NbtIntArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        void visit(NbtIntNode *node) final {
            m_repr += QString("NbtIntNode(%1)").arg(node->value());
        };
        void visit(NbtListNode *node) final {
            m_repr += QString("NbtListNode[%1]").arg(node->size());
            reprList(node->children());
        };
        void visit(NbtLongArrayNode *node) final {
            m_repr += QString("NbtLongArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        void visit(NbtLongNode *node) final {
            m_repr += QString("NbtLongNode(%1)").arg(node->value());
        };
        void visit(NbtShortNode *node) final {
            m_repr += QString("NbtShortNode(%1)").arg(node->value());
        };
        void visit(NbtStringNode *node) final {
            m_repr += QString("NbtStringNode(\"%1\")").arg(node->value());
        };
        void visit(NbtPathNode *node) final {
            m_repr += QString("NbtPathNode[%1]").arg(node->size());
            reprList(node->steps());
        };
        void visit(NbtPathStepNode *node)  final {
            using Type = NbtPathStepNode::Type;
            static const QMap<Type, QString> type2Name {
                { Type::Index, "Index" },
                { Type::Key, "Key" },
                { Type::Root, "Root" },
            };

            m_repr += QString("NbtPathStepNode<%1>(").arg(
                type2Name[node->type()]);

            switch (node->type()) {
                case Type::Root: {
                    m_repr += '{';
                    if (node->filter())
                        node->filter()->accept(this, m_order);
                    m_repr += '}';
                    break;
                }

                case Type::Key: {
                    node->name()->accept(this, m_order);

                    if (node->filter()) {
                        m_repr += '{';
                        node->filter()->accept(this, m_order);
                        m_repr += '}';
                    }
                    break;
                }

                case Type::Index: {
                    m_repr += '[';
                    if (node->filter()) {
                        m_repr += '{';
                        node->filter()->accept(this, m_order);
                        m_repr += '}';
                    }else if (node->index()) {
                        node->index()->accept(this, m_order);
                    }
                    m_repr += ']';
                    break;
                }
            }
            if (node->trailingTrivia() == '.')
                m_repr += '.';

            m_repr += ')';
        };
        void visit(ResourceLocationNode *node) final {
            m_repr += "ResourceLocationNode";
            reprResourceLocation(node);
        };
        void visit(ResourceNode *node) final {
            m_repr += "ResourceNode";
            reprResourceLocation(node);
        };
        void visit(ResourceKeyNode *node) final {
            m_repr += "ResourceKeyNode";
            reprResourceLocation(node);
        };
        void visit(ResourceOrTagNode *node) final {
            m_repr += "ResourceOrTagNode";
            reprResourceLocation(node);
        };
        void visit(ResourceOrTagKeyNode *node) final {
            m_repr += "ResourceOrTagKeyNode";
            reprResourceLocation(node);
        };
        void visit(BlockPosNode *node) final {
            m_repr += "BlockPosNode";
            reprAxes(node);
        }
        void visit(BlockPredicateNode *node) final {
            m_repr += "BlockPredicateNode(";
            node->resLoc()->accept(this, m_order);
            m_repr += ')';
            if (node->states() && !node->states()->isEmpty()) {
                m_repr += '[';
                node->states()->accept(this, m_order);
                m_repr += ']';
            }
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        void visit(ColumnPosNode *node) final {
            m_repr += "ColumnPosNode";
            reprAxes(node, "x", "z");
        }
        void visit(RotationNode *node) final {
            m_repr += "RotationNode";
            reprAxes(node, "yaw", "pitch");
        }
        void visit(Vec2Node *node) final {
            m_repr += "Vec2Node";
            reprAxes(node, "x", "y");
        }
        void visit(Vec3Node *node) final {
            m_repr += "Vec3Node";
            reprAxes(node);
        }
        void visit(DimensionNode *node) final {
            m_repr += "DimensionNode";
            reprResourceLocation(node);
        };
        void visit(EntitySummonNode *node) final {
            m_repr += "EntitySummonNode";
            reprResourceLocation(node);
        };
        void visit(FunctionNode *node) final {
            m_repr += "FunctionNode";
            reprResourceLocation(node);
        };
        void visit(ItemEnchantmentNode *node) final {
            m_repr += "ItemEnchantmentNode";
            reprResourceLocation(node);
        };
        void visit(MobEffectNode *node) final {
            m_repr += "MobEffectNode";
            reprResourceLocation(node);
        };
        void visit(ColorNode *node) final {
            m_repr += QString("ColorNode(\"%1\")").arg(node->value());
        };
        void visit(EntityAnchorNode *node) final {
            m_repr += QString("EntityAnchorNode(\"%1\")").arg(node->value());
        };
        void visit(HeightmapNode *node) final {
            m_repr += QString("HeightmapNode(\"%1\")").arg(node->value());
        };
        void visit(ItemSlotNode *node) final {
            m_repr += QString("ItemSlotNode(\"%1\")").arg(node->value());
        };
        void visit(ItemSlotsNode *node) final {
            m_repr += QString("ItemSlotsNode(\"%1\")").arg(node->value());
        };
        void visit(MessageNode *node) final {
            m_repr += QString("MessageNode(\"%1\")").arg(node->value());
        };
        void visit(ObjectiveNode *node) final {
            m_repr += QString("ObjectiveNode(\"%1\")").arg(node->value());
        };
        void visit(ObjectiveCriteriaNode *node) final {
            m_repr +=
                QString("ObjectiveCriteriaNode(\"%1\")").arg(node->value());
        };
        void visit(OperationNode *node) final {
            m_repr += QString("OperationNode(\"%1\")").arg(node->value());
        };
        void visit(ScoreboardSlotNode *node) final {
            m_repr += QString("ScoreboardSlotNode(\"%1\")").arg(node->value());
        };
        void visit(TeamNode *node) final {
            m_repr += QString("TeamNode(\"%1\")").arg(node->value());
        };
        void visit(TemplateMirrorNode *node) final {
            m_repr += QString("TemplateMirrorNode(\"%1\")").arg(node->value());
        };
        void visit(TemplateRotationNode *node) final {
            m_repr +=
                QString("TemplateRotationNode(\"%1\")").arg(node->value());
        };
        void visit(SwizzleNode *node) final {
            m_repr += QString("SwizzleNode(%1)").arg(node->text());
        };
        void visit(TargetSelectorNode *node) final {
            using Variable = TargetSelectorNode::Variable;
            const static QMap<TargetSelectorNode::Variable, char> variableMap =
            {   { Variable::A, 'a' },
                { Variable::E, 'e' },
                { Variable::P, 'p' },
                { Variable::R, 'r' },
                { Variable::S, 's' } };

            m_repr +=
                QString("TargetSelectorNode(@%1)").arg(variableMap[node->
                                                                   variable()]);

            if (node->args()) {
                m_repr += '{';
                node->args()->accept(this, m_order);
                m_repr += '}';
            }
        };
        void visit(TimeNode *node) final {
            m_repr += QString("TimeNode(%1)").arg(node->text());
        };
        void visit(UuidNode *node) final {
            m_repr +=
                QString("UuidNode(%1)").arg(node->value().toString());
        };
        void visit(ParticleNode *node) final {
            m_repr += "ParticleNode(id: ";
            node->resLoc()->accept(this, m_order);
            if (const auto options = node->options()) {
                m_repr += ", options: ";
                options->accept(this, m_order);
            } else if (!node->params().isEmpty()) {
                m_repr += ", params: ";
                reprList(node->params());
            }
            m_repr += ')';
        }
        void visit(ParticleColorNode *node) final {
            m_repr += "ParticleColorNode(";
            node->r()->accept(this, m_order);
            m_repr += ", ";
            node->g()->accept(this, m_order);
            m_repr += ", ";
            node->b()->accept(this, m_order);
            m_repr += ')';
        };
        void visit(ItemPredicateNode *node) final {
            m_repr += "ItemPredicateNode(";
            node->resLoc()->accept(this, m_order);
            m_repr += ')';
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        void visit(KeyNode *node) final {
            m_repr += QString("KeyNode(\"%1\")").arg(node->value());
        };
        void visit(TwoAxesNode *node) final {
            m_repr += "TwoAxesNode";
            reprAxes(node, "first", "second");
        }
        void visit(XyzNode *node) final {
            m_repr += "XyzNode";
            reprAxes(node);
        }
        void visit(GamemodeNode *node) final {
            m_repr += QString("GamemodeNode(\"%1\")").arg(node->text());
        };
        void visit(InlinableResourceNode *node) {
            m_repr += "InlinableResourceNode(";
            node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        void visit(LootModifierNode *node) {
            m_repr += "LootModifierNode(";
            node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        void visit(LootPredicateNode *node) {
            m_repr += "LootPredicateNode(";
            node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        void visit(LootTableNode *node) {
            m_repr += "LootTableNode(";
            node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        void visit(InternalGreedyStringNode *node) final {
            m_repr += QString("InternalGreedyStringNode(\"%1\")").arg(
                node->text());
        };
        void visit(InternalRegexPatternNode *node) final {
            m_repr += QString("InternalRegexPatternNode(%1)").arg(
                node->value().pattern());
        };

        QString repr() const;

private:
        QString m_repr;

        template<class T>
        void reprList(const T &vector, bool autoOpen = true) {
            if (autoOpen)
                m_repr += '(';
            for (const auto &item: vector) {
                item->accept(this, m_order);
                if (item != vector.constLast()) {
                    m_repr += ", ";
                }
            }
            m_repr += ')';
        }

        template<class T>
        typename std::enable_if<std::is_base_of<TwoAxesNode, T>::value,
                                void>::type
        reprAxes(T *node, const char *firstLabel, const char *secondLabel) {
            m_repr += '(';
            m_repr += QLatin1String(firstLabel);
            m_repr += ": ";
            node->firstAxis()->accept(this, m_order);
            m_repr += ", ";
            m_repr += QLatin1String(secondLabel);
            m_repr += ": ";
            node->secondAxis()->accept(this, m_order);
            m_repr += ')';
        };

        template<class T>
        typename std::enable_if<std::is_base_of<XyzNode, T>::value, void>::type
        reprAxes(T *node) {
            m_repr += "(x: ";
            node->x()->accept(this, m_order);
            m_repr += ", y: ";
            node->y()->accept(this, m_order);
            m_repr += ", z: ";
            node->z()->accept(this, m_order);
            m_repr += ')';
        };

        void reprResourceLocation(ResourceLocationNode const *node);
        void reprEntityNode(EntityNode const *node);
    };
}

#endif // REPRPRINTER_H
