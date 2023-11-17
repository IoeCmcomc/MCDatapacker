#ifndef SOURCEPRINTER_H
#define SOURCEPRINTER_H

#include "overloadnodevisitor.h"

namespace Command {
    class SourcePrinter : public OverloadNodeVisitor {
public:
        SourcePrinter();

        void visit(ParseNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            m_text += node->text();
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void visit(RootNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            for (const auto &child: node->children()) {
                child->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(BlockStateNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->resLoc()->accept(this, m_order);
            if (node->states()) {
                node->states()->accept(this, m_order);
            }
            if (node->nbt()) {
                node->nbt()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(EntityNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->getNode()->accept(this, m_order);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(ScoreHolderNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->isAll()) {
                m_text += '*';
            } else {
                node->getNode()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(FloatRangeNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->hasMinValue() || node->hasMaxValue()) {
                if (node->hasMinValue()) {
                    node->minValue()->accept(this, m_order);
                }
                if (node->hasMaxValue()) {
                    node->maxValue()->accept(this, m_order);
                }
            } else {
                node->exactValue()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(IntRangeNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->hasMinValue() || node->hasMaxValue()) {
                if (node->hasMinValue()) {
                    node->minValue()->accept(this, m_order);
                }
                if (node->hasMaxValue()) {
                    node->maxValue()->accept(this, m_order);
                }
            } else {
                node->exactValue()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(ItemStackNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->resLoc()->accept(this, m_order);
            if (node->nbt()) {
                node->nbt()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(MapNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                m_text += i->get()->leadingTrivia();
                m_text += i->get()->leftText();
                i->get()->first->accept(this, m_order);
                i->get()->second->accept(this, m_order);
                m_text += i->get()->rightText();
                m_text += i->get()->trailingTrivia();
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(EntityArgumentValueNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->getNode()) {
                node->getNode()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(NbtByteArrayNode *node) override {
            printList(node, node->children());
        };
        void visit(NbtCompoundNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                m_text += i->get()->leadingTrivia();
                m_text += i->get()->leftText();
                i->get()->first->accept(this, m_order);
                i->get()->second->accept(this, m_order);
                m_text += i->get()->rightText();
                m_text += i->get()->trailingTrivia();
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(NbtIntArrayNode *node) override {
            printList(node, node->children());
        };
        void visit(NbtListNode *node) override {
            printList(node, node->children());
        };
        void visit(NbtLongArrayNode *node) override {
            printList(node, node->children());
        };
        void visit(NbtPathNode *node) override {
            printList(node, node->steps());
        };
        void visit(NbtPathStepNode *node) override {
            using Type = NbtPathStepNode::Type;

            m_text += node->leadingTrivia();
            m_text += node->leftText();

            switch (node->type()) {
                case Type::Root: {
                    if (node->filter())
                        node->filter()->accept(this, m_order);
                    break;
                }

                case Type::Key: {
                    node->name()->accept(this, m_order);

                    if (node->filter()) {
                        node->filter()->accept(this, m_order);
                    }
                    break;
                }

                case Type::Index: {
                    if (node->filter()) {
                        node->filter()->accept(this, m_order);
                    }else if (node->index()) {
                        node->index()->accept(this, m_order);
                    }
                    break;
                }
            }

            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(ResourceLocationNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();

            if (node->nspace()) {
                m_text += node->nspace()->leadingTrivia();
                m_text += node->nspace()->leftText();
                m_text += node->nspace()->text();
                m_text += node->nspace()->rightText();
                m_text += node->nspace()->trailingTrivia();
            }
            m_text += node->id()->leadingTrivia();
            m_text += node->id()->leftText();
            m_text += node->id()->text();
            m_text += node->id()->rightText();
            m_text += node->id()->trailingTrivia();

            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void visit(TwoAxesNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->firstAxis()->accept(this, m_order);
            node->secondAxis()->accept(this, m_order);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void visit(XyzNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->x()->accept(this, m_order);
            node->y()->accept(this, m_order);
            node->z()->accept(this, m_order);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void visit(TargetSelectorNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->args()) {
                node->args()->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };


        void visit(ParticleColorNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->r()->accept(this, m_order);
            node->g()->accept(this, m_order);
            node->b()->accept(this, m_order);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void visit(ParticleNode *node) override {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->resLoc()->accept(this, m_order);
            printList(node->params());
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        template <typename V>
        void printList(const V &vector) {
            for (const auto &item: vector) {
                item->accept(this, m_order);
            }
        }

        template <class T, typename V>
        void printList(T *node, const V &vector) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            for (const auto &item: vector) {
                item->accept(this, m_order);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        }

        QString source() const {
            return m_text;
        }

private:
        QString m_text;
    };
}

#endif // SOURCEPRINTER_H
