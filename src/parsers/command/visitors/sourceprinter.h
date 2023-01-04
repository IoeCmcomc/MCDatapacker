#ifndef SOURCEPRINTER_H
#define SOURCEPRINTER_H

#include "overloadnodevisitor.h"

namespace Command {
    struct SourcePrinterImpl {
        void operator()(ParseNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            m_text += node->text();
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(RootNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            for (const auto &child: node->children()) {
                child->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(BlockStateNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            printResourceLocation(node);
            if (node->states()) {
                node->states()->accept(visitor, LetTheVisitorDecide);
            }
            if (node->nbt()) {
                node->nbt()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(EntityNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->getNode()->accept(visitor, LetTheVisitorDecide);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(FloatRangeNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->hasMinValue() || node->hasMaxValue()) {
                if (node->hasMinValue()) {
                    node->minValue()->accept(visitor, LetTheVisitorDecide);
                }
                if (node->hasMaxValue()) {
                    node->maxValue()->accept(visitor, LetTheVisitorDecide);
                }
            } else {
                node->exactValue()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(IntRangeNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->hasMinValue() || node->hasMaxValue()) {
                if (node->hasMinValue()) {
                    node->minValue()->accept(visitor, LetTheVisitorDecide);
                }
                if (node->hasMaxValue()) {
                    node->maxValue()->accept(visitor, LetTheVisitorDecide);
                }
            } else {
                node->exactValue()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(ItemStackNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            printResourceLocation(node);
            if (node->nbt()) {
                node->nbt()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(MapNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                m_text += i->get()->leadingTrivia();
                m_text += i->get()->leftText();
                i->get()->first->accept(visitor, LetTheVisitorDecide);
                i->get()->second->accept(visitor, LetTheVisitorDecide);
                m_text += i->get()->rightText();
                m_text += i->get()->trailingTrivia();
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(EntityArgumentValueNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->getNode()) {
                node->getNode()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(NbtByteArrayNode *node) {
            printList(node, node->children());
        };
        void operator()(NbtCompoundNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            const auto &&pairs = node->pairs();
            for (auto i = pairs.cbegin(); i != pairs.cend(); ++i) {
                m_text += i->get()->leadingTrivia();
                m_text += i->get()->leftText();
                i->get()->first->accept(visitor, LetTheVisitorDecide);
                i->get()->second->accept(visitor, LetTheVisitorDecide);
                m_text += i->get()->rightText();
                m_text += i->get()->trailingTrivia();
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(NbtIntArrayNode *node) {
            printList(node, node->children());
        };
        void operator()(NbtListNode *node) {
            printList(node, node->children());
        };
        void operator()(NbtLongArrayNode *node) {
            printList(node, node->children());
        };
        void operator()(NbtPathNode *node) {
            printList(node, node->steps());
        };
        void operator()(NbtPathStepNode *node) {
            using Type = NbtPathStepNode::Type;
            static const QMap<Type, QString> type2Name {
                { Type::Index, "Index" },
                { Type::Key, "Key" },
                { Type::Root, "Root" },
            };

            m_text += node->leadingTrivia();
            m_text += node->leftText();

            switch (node->type()) {
                case Type::Root: {
                    if (node->filter())
                        node->filter()->accept(visitor, LetTheVisitorDecide);
                    break;
                }

                case Type::Key: {
                    node->name()->accept(visitor, LetTheVisitorDecide);

                    if (node->filter()) {
                        node->filter()->accept(visitor, LetTheVisitorDecide);
                    }
                    break;
                }

                case Type::Index: {
                    if (node->filter()) {
                        node->filter()->accept(visitor, LetTheVisitorDecide);
                    }else if (node->index()) {
                        node->index()->accept(visitor, LetTheVisitorDecide);
                    }
                    break;
                }
            }

            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(ResourceLocationNode *node) {
            printResourceLocation(node);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void operator()(TwoAxesNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->firstAxis()->accept(visitor, LetTheVisitorDecide);
            node->secondAxis()->accept(visitor, LetTheVisitorDecide);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void operator()(XyzNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->x()->accept(visitor, LetTheVisitorDecide);
            node->y()->accept(visitor, LetTheVisitorDecide);
            node->z()->accept(visitor, LetTheVisitorDecide);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void operator()(TargetSelectorNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            if (node->args()) {
                node->args()->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };


        void operator()(ParticleColorNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            node->r()->accept(visitor, LetTheVisitorDecide);
            node->g()->accept(visitor, LetTheVisitorDecide);
            node->b()->accept(visitor, LetTheVisitorDecide);
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };
        void operator()(ParticleNode *node) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            printResourceLocation(node);
            printList(node->params());
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        };

        void printResourceLocation(ResourceLocationNode *node) {
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
        }

        template <typename V>
        void printList(const V &vector) {
            for (const auto &item: vector) {
                item->accept(visitor, LetTheVisitorDecide);
            }
        }

        template <class T, typename V>
        void printList(T *node, const V &vector) {
            m_text += node->leadingTrivia();
            m_text += node->leftText();
            for (const auto &item: vector) {
                item->accept(visitor, LetTheVisitorDecide);
            }
            m_text += node->rightText();
            m_text += node->trailingTrivia();
        }

        NodeVisitor *visitor = nullptr;
        QString      m_text;
    };


    class SourcePrinter : public OverloadNodeVisitor<SourcePrinterImpl> {
public:
        SourcePrinter();

        void startVisiting(ParseNode *node) override {
            node->accept(this, m_order);
        }

        QString source() const {
            return impl.m_text;
        }
    };
}

#endif // SOURCEPRINTER_H
