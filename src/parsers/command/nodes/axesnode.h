#ifndef AXESNODE_H
#define AXESNODE_H

#include "anglenode.h"

namespace Command {
    using AnglePtr = QSharedPointer<AngleNode>;

    class TwoAxesNode : public ArgumentNode
    {
public:
        AnglePtr firstAxis() const;
        void setFirstAxis(AnglePtr axis);

        AnglePtr secondAxis() const;
        void setSecondAxis(AnglePtr axis);

protected:
        explicit TwoAxesNode(ParserType parserType, int length);

        void _accept(NodeVisitor *visitor, VisitOrder order);

private:
        AnglePtr m_first  = nullptr;
        AnglePtr m_second = nullptr;
    };

    class XyzNode : public ArgumentNode
    {
public:
        AnglePtr y();
        void setY(AnglePtr y);

        AnglePtr x() const;
        void setX(AnglePtr x);

        AnglePtr z() const;
        void setZ(AnglePtr z);

protected:
        explicit XyzNode(ParserType parserType, int length);

        void _accept(NodeVisitor *visitor, VisitOrder order);

private:
        AnglePtr m_x = nullptr;
        AnglePtr m_y = nullptr;
        AnglePtr m_z = nullptr;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::TwoAxesNode>)
Q_DECLARE_METATYPE(QSharedPointer<Command::XyzNode>)

#define DECLARE_NODE_CLASS(Name, Base)                                   \
        namespace Command {                                              \
            class Name ## Node : public Base {                           \
public:                                                                  \
                explicit Name ## Node(int length) : Base(                \
                        ArgumentNode::ParserType::Name, length) {}       \
                void accept(NodeVisitor * visitor, VisitOrder) override; \
            };                                                           \
        }                                                                \
        Q_DECLARE_METATYPE(QSharedPointer<Command::Name ## Node>)        \
        const static bool _ ## Name =                                    \
            TypeRegister<Command::Name ## Node>::init();                 \

DECLARE_NODE_CLASS(BlockPos, XyzNode)
DECLARE_NODE_CLASS(ColumnPos, TwoAxesNode)
DECLARE_NODE_CLASS(Rotation, TwoAxesNode)
DECLARE_NODE_CLASS(Vec2, TwoAxesNode)
DECLARE_NODE_CLASS(Vec3, XyzNode)

#undef DECLARE_NODE_CLASS

#endif /* AXESNODE_H */
