#ifndef SINGLEVALUENODE_H
#define SINGLEVALUENODE_H

#include "argumentnode.h"
#include <QUuid>

namespace Command {
    template <class Base, typename T, ArgumentNode::ParserType PT>
    class SingleValueNode : public Base
    {
public:
        SingleValueNode(const QString &text, const T &value)
            : Base(PT, text), m_value(value) {
            this->m_isValid = true;
        };
        template <typename _T = T,
                  typename = typename std::enable_if_t<std::is_same<_T,
                                                                    QString>::value> >
        explicit SingleValueNode(const QString &text)
            : Base(PT, text), m_value(text) {
            this->m_isValid = true;
        };

        inline T value() const {
            return m_value;
        }

        inline void setValue(const T &newValue) {
            m_value = newValue;
        }

protected:
        T m_value{};
    };
}

// Template classes can't be forward declared easily, therefore creating subclasses is necessary.
#define DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Name, T)                                    \
        namespace Command {                                                             \
            class Name ## Node : public SingleValueNode<ArgumentNode, T,                \
                                                        ArgumentNode::ParserType::Name> \
            {                                                                           \
public:                                                                                 \
                using SingleValueNode::SingleValueNode;                                 \
                void accept(NodeVisitor * visitor, VisitOrder) override;                \
            };                                                                          \
            DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Name)                           \
        }                                                                               \

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Bool, bool)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Double, double)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Float, float)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Integer, int)

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Color, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(EntityAnchor, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ItemSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Message, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Objective, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ObjectiveCriteria, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Operation, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ScoreboardSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Team, QString)

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Uuid, QUuid)

#undef DECLARE_SINGLE_VALUE_ARGUMENT_CLASS

#endif // SINGLEVALUENODE_H
