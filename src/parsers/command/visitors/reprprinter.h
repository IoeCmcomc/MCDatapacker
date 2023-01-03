#ifndef REPRPRINTER_H
#define REPRPRINTER_H

#include "nodevisitor.h"

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

namespace Command {
    class ReprPrinter : public NodeVisitor
    {
public:
        explicit ReprPrinter();

        virtual void startVisiting(ParseNode *node) override {
            node->accept(this, m_order);
        };

        virtual void visit(ParseNode *node) override {
            m_repr += QString("ParseNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown ParseNode detected.";
        };
        virtual void visit(ErrorNode *node) override {
            m_repr += QString("ErrorNode[%1]").arg(node->length());
        };
        virtual void visit(RootNode *node) override {
            m_repr += QString("RootNode[%1](").arg(node->size());
            for (const auto &child: node->children()) {
                child->accept(this, m_order);
                if (child != node->children().back()) {
                    m_repr += ", ";
                }
            }
            m_repr += ')';
        };
        virtual void visit(ArgumentNode *node) override {
            m_repr += QString("ArgumentNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown ArgumentNode detected.";
        };
        virtual void visit(BoolNode *node) override {
            m_repr += QString("BoolNode(%1)").arg(
                node->value() ? "true" : "false");
        };
        virtual void visit(DoubleNode *node) override {
            m_repr += QString("DoubleNode(%1)").arg(node->value());
        };
        virtual void visit(FloatNode *node) override {
            m_repr += QString("FloatNode(%1)").arg(node->value());
        };
        virtual void visit(IntegerNode *node) override {
            m_repr += QString("IntegerNode(%1)").arg(node->value());
        };
        virtual void visit(StringNode *node) override {
            m_repr += QString("StringNode(\"%1\")").arg(node->value());
        };
        virtual void visit(LiteralNode *node) override {
            m_repr += QString("LiteralNode(%1)").arg(node->text());
        };
        virtual void visit(AngleNode *node) override {
            m_repr += QString("AngleNode(%1)").arg(node->text());
        };
        virtual void visit(BlockStateNode *node) override {
            m_repr += "BlockStateNode";
            reprResourceLocation(node);
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
        virtual void visit(ComponentNode *node) override {
            m_repr += QString("ComponentNode(%1)").arg(
                node->value().dump().c_str());
        };
        virtual void visit(EntityNode *node) override {
            m_repr += "EntityNode";
            reprEntityNode(node);
        };
        virtual void visit(GameProfileNode *node) override {
            m_repr += "GameProfileNode";
            reprEntityNode(node);
        };
        virtual void visit(ScoreHolderNode *node) override {
            m_repr += "ScoreHolderNode";
            reprEntityNode(node);
        };
        virtual void visit(FloatRangeNode *node) override {
            m_repr += QString("FloatRangeNode(%1)").arg(node->format());
        };
        virtual void visit(IntRangeNode *node) override {
            m_repr += QString("IntRangeNode(%1)").arg(node->format());
        };
        virtual void visit(ItemStackNode *node) override {
            m_repr += "ItemStackNode";
            reprResourceLocation(node);
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        virtual void visit(MapNode *node) override {
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
        virtual void visit(EntityArgumentValueNode *node) override {
            m_repr += "EntityArgumentValueNode";
            if (node->isNegative())
                m_repr += "[!]";
            m_repr += '(';
            if (node->getNode())
                node->getNode()->accept(this, m_order);
            m_repr += ')';
        }
        virtual void visit(NbtNode *node) override {
            m_repr += QString("NbtNode[%1]").arg(node->length());
            qWarning() << "ReprPrinter: Unknown NbtNode detected.";
        };
        virtual void visit(NbtByteArrayNode *node) override {
            m_repr += QString("NbtByteArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        virtual void visit(NbtByteNode *node) override {
            m_repr += QString("NbtByteNode(%1)").arg(node->value());
        };
        virtual void visit(NbtCompoundNode *node) override {
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
        virtual void visit(NbtDoubleNode *node) override {
            m_repr += QString("NbtDoubleNode(%1)").arg(node->value());
        };
        virtual void visit(NbtFloatNode *node) override {
            m_repr += QString("NbtFloatNode(%1)").arg(node->value());
        };
        virtual void visit(NbtIntArrayNode *node) override {
            m_repr += QString("NbtIntArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        virtual void visit(NbtIntNode *node) override {
            m_repr += QString("NbtIntNode(%1)").arg(node->value());
        };
        virtual void visit(NbtListNode *node) override {
            m_repr += QString("NbtListNode[%1]").arg(node->size());
            reprList(node->children());
        };
        virtual void visit(NbtLongArrayNode *node) override {
            m_repr += QString("NbtLongArrayNode[%1]").arg(node->size());
            reprList(node->children());
        };
        virtual void visit(NbtLongNode *node) override {
            m_repr += QString("NbtLongNode(%1)").arg(node->value());
        };
        virtual void visit(NbtShortNode *node) override {
            m_repr += QString("NbtShortNode(%1)").arg(node->value());
        };
        virtual void visit(NbtStringNode *node) override {
            m_repr += QString("NbtStringNode(\"%1\")").arg(node->value());
        };
        virtual void visit(NbtPathNode *node) override {
            m_repr += QString("NbtPathNode[%1]").arg(node->size());
            reprList(node->steps());
        };
        virtual void visit(NbtPathStepNode *node)  override {
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
        virtual void visit(ResourceLocationNode *node) override {
            m_repr += "ResourceLocationNode";
            reprResourceLocation(node);
        };
        virtual void visit(BlockPosNode *node) override {
            m_repr += "BlockPosNode";
            reprAxes(node);
        }
        virtual void visit(BlockPredicateNode *node) override {
            m_repr += "BlockPredicateNode";
            reprResourceLocation(node);
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
        virtual void visit(ColumnPosNode *node) override {
            m_repr += "ColumnPosNode";
            reprAxes(node, "x", "z");
        }
        virtual void visit(RotationNode *node) override {
            m_repr += "RotationNode";
            reprAxes(node, "yaw", "pitch");
        }
        virtual void visit(Vec2Node *node) override {
            m_repr += "Vec2Node";
            reprAxes(node, "x", "y");
        }
        virtual void visit(Vec3Node *node) override {
            m_repr += "Vec3Node";
            reprAxes(node);
        }
        virtual void visit(DimensionNode *node) override {
            m_repr += "DimensionNode";
            reprResourceLocation(node);
        };
        virtual void visit(EntitySummonNode *node) override {
            m_repr += "EntitySummonNode";
            reprResourceLocation(node);
        };
        virtual void visit(FunctionNode *node) override {
            m_repr += "FunctionNode";
            reprResourceLocation(node);
        };
        virtual void visit(ItemEnchantmentNode *node) override {
            m_repr += "ItemEnchantmentNode";
            reprResourceLocation(node);
        };
        virtual void visit(MobEffectNode *node) override {
            m_repr += "MobEffectNode";
            reprResourceLocation(node);
        };
        virtual void visit(ColorNode *node) override {
            m_repr += QString("ColorNode(\"%1\")").arg(node->value());
        };
        virtual void visit(EntityAnchorNode *node) override {
            m_repr += QString("EntityAnchorNode(\"%1\")").arg(node->value());
        };
        virtual void visit(ItemSlotNode *node) override {
            m_repr += QString("ItemSlotNode(\"%1\")").arg(node->value());
        };
        virtual void visit(MessageNode *node) override {
            m_repr += QString("MessageNode(\"%1\")").arg(node->value());
        };
        virtual void visit(ObjectiveNode *node) override {
            m_repr += QString("ObjectiveNode(\"%1\")").arg(node->value());
        };
        virtual void visit(ObjectiveCriteriaNode *node) override {
            m_repr +=
                QString("ObjectiveCriteriaNode(\"%1\")").arg(node->value());
        };
        virtual void visit(OperationNode *node) override {
            m_repr += QString("OperationNode(\"%1\")").arg(node->value());
        };
        virtual void visit(ScoreboardSlotNode *node) override {
            m_repr += QString("ScoreboardSlotNode(\"%1\")").arg(node->value());
        };
        virtual void visit(TeamNode *node) override {
            m_repr += QString("TeamNode(\"%1\")").arg(node->value());
        };
        virtual void visit(SwizzleNode *node) override {
            m_repr += QString("SwizzleNode(%1)").arg(node->text());
        };
        virtual void visit(TargetSelectorNode *node) override {
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
        virtual void visit(TimeNode *node) override {
            m_repr += QString("TimeNode(%1)").arg(node->text());
        };
        virtual void visit(UuidNode *node) override {
            m_repr +=
                QString("UuidNode(%1)").arg(node->value().toString());
        };
        virtual void visit(ParticleNode *node) override {
            m_repr += "ParticleNode";
            reprResourceLocation(node, false);
            if (!node->params().isEmpty()) {
                m_repr += ", params: ";
                reprList(node->params());
            }
            m_repr += ')';
        }
        virtual void visit(ParticleColorNode *node) override {
            m_repr += "ParticleColorNode(";
            node->r()->accept(this, m_order);
            m_repr += ", ";
            node->g()->accept(this, m_order);
            m_repr += ", ";
            node->b()->accept(this, m_order);
            m_repr += ')';
        };
        virtual void visit(ItemPredicateNode *node) override {
            m_repr += "ItemPredicateNode";
            reprResourceLocation(node);
            if (node->nbt() && !node->nbt()->isEmpty()) {
                m_repr += '{';
                node->nbt()->accept(this, m_order);
                m_repr += '}';
            }
        };
        virtual void visit(KeyNode *node) override {
            m_repr += QString("KeyNode(\"%1\")").arg(node->value());
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
        reprAxes(T *node, const QString &firstLabel,
                 const QString &secondLabel) {
            m_repr += '(';
            m_repr += firstLabel;
            m_repr += ": ";
            node->firstAxis()->accept(this, m_order);
            m_repr += ", ";
            m_repr += secondLabel;
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

        void reprResourceLocation(ResourceLocationNode *node,
                                  bool autoClose = true);
        void reprEntityNode(EntityNode *node);
    };
}

#endif // REPRPRINTER_H
