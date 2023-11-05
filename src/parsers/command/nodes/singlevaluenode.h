#ifndef SINGLEVALUENODE_H
#define SINGLEVALUENODE_H

#include "argumentnode.h"

#include <QUuid>
#include <QRegularExpression>

namespace Command {
    template <class Base, typename T, ArgumentNode::ParserType PT>
    class SingleValueNode : public Base {
public:
        SingleValueNode(const QString &text, const T &value,
                        const bool isValid = false)
            : Base(PT, text), m_value(value) {
            this->m_isValid = isValid;
        };
        template <typename _T = T,
                  typename = typename std::enable_if_t<std::is_same<_T,
                                                                    QString>::value> >
        explicit SingleValueNode(const QString &text,
                                 const bool isValid = false)
            : Base(PT, text), m_value(text) {
            this->m_isValid = isValid;
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

/*
 * Template classes can't be forward declared easily and can't have virtual overrides,
 * therefore creating subclasses is necessary.
 */
#define DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Name, T)                                    \
        namespace Command {                                                             \
            class Name ## Node : public SingleValueNode<ArgumentNode, T,                \
                                                        ArgumentNode::ParserType::Name> \
            {                                                                           \
public:                                                                                 \
                using SingleValueNode::SingleValueNode;                                 \
                void accept(NodeVisitor * visitor, VisitOrder) final;                   \
            };                                                                          \
            DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Name)                           \
        }                                                                               \

namespace Command {
    class FloatNode : public SingleValueNode<ArgumentNode, float,
                                             ArgumentNode::ParserType::Float>
    {
public:
        using SingleValueNode::SingleValueNode;
        void accept(NodeVisitor *visitor, VisitOrder) final;
        void chopTrailingDot();
    };
    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Float)
}

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Bool, bool)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Double, double)
//DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Float, float)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Integer, int)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Long, long long)

namespace Command {
    template<>
    constexpr std::array staticSuggestions<BoolNode> = {
        "false"_QL1, "true"_QL1,
    };
}

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Color, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(EntityAnchor, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Heightmap, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ItemSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Message, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Objective, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ObjectiveCriteria, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Operation, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(ScoreboardSlot, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Team, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(TemplateMirror, QString)
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(TemplateRotation, QString)

DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(Uuid, QUuid)

namespace Command {
    template<>
    constexpr std::array staticSuggestions<ColorNode> = {
        "aqua"_QL1,         "black"_QL1,       "blue"_QL1,
        "dark_aqua"_QL1,    "dark_blue"_QL1,   "dark_green"_QL1,
        "dark_gray"_QL1,    "dark_purple"_QL1, "dark_red"_QL1,
        "gold"_QL1,         "green"_QL1,       "gray"_QL1,
        "light_purple"_QL1, "red"_QL1,         "reset"_QL1,
        "white"_QL1,        "yellow"_QL1,
    };

    template<>
    constexpr std::array staticSuggestions<EntityAnchorNode> =
    { "eyes"_QL1, "feet"_QL1, };

    template<>
    constexpr std::array staticSuggestions<HeightmapNode> = {
        "motion_blocking_no_leaves"_QL1, "motion_blocking"_QL1,
        "ocean_floor"_QL1,               "world_surface"_QL1,
    };

    template<>
    constexpr std::array staticSuggestions<OperationNode> = {
        "="_QL1,  "<"_QL1,  ">"_QL1, "><"_QL1, "+="_QL1, "-="_QL1, "*="_QL1,
        "/="_QL1, "%="_QL1,
    };

    template<>
    constexpr std::array staticSuggestions<ScoreboardSlotNode> = {
        "belowName"_QL1, // Will be renamed to below_name in 1.20.2
        "sidebar.team.aqua"_QL1,
        "sidebar.team.black"_QL1,
        "sidebar.team.blue"_QL1,
        "sidebar.team.dark_aqua"_QL1,
        "sidebar.team.dark_blue"_QL1,
        "sidebar.team.dark_green"_QL1,
        "sidebar.team.dark_gray"_QL1,
        "sidebar.team.dark_purple"_QL1,
        "sidebar.team.dark_red"_QL1,
        "sidebar.team.gold"_QL1,
        "sidebar.team.green"_QL1,
        "sidebar.team.gray"_QL1,
        "sidebar.team.light_purple"_QL1,
        "sidebar.team.red"_QL1,
        "sidebar.team.reset"_QL1,
        "sidebar.team.white"_QL1,
        "sidebar.team.yellow"_QL1,
        "sidebar"_QL1,
        "list"_QL1,
    };

    template<uint N>
    constexpr auto changeArrayValue(std::array<QLatin1String, N> arr,
                                    const int i, QLatin1String str) {
        arr[i] = str;
        return arr;
    }

    constexpr std::array staticSuggestions_ScoreboardSlotNode_v1_20_2 =
        changeArrayValue(staticSuggestions<ScoreboardSlotNode>, 0,
                         "below_name"_QL1);

    template<>
    constexpr std::array staticSuggestions<TemplateMirrorNode> = {
        "none"_QL1, "front_back"_QL1, "left_right"_QL1,
    };

    template<>
    constexpr std::array staticSuggestions<TemplateRotationNode> = {
        "none"_QL1, "clockwise_90"_QL1, "counterclockwise_90"_QL1, "180"_QL1
    };
}

/*
 * InternalGreedyStringNode represent a unquoted string contains characters
 * except whitespaces, quote punctiations and backslashes.
 *
 * Namespaced ID: ___:greedy_string
 */
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(InternalGreedyString, QString)
/*
 * InternalRegexPatternNode represent a regular expression pattern.
 * Agument nodes of this type must be put at the end of a command branch.
 *
 * Namespaced ID: ___:regex_pattern
 */
DECLARE_SINGLE_VALUE_ARGUMENT_CLASS(InternalRegexPattern, QRegularExpression)

#undef DECLARE_SINGLE_VALUE_ARGUMENT_CLASS

#endif // SINGLEVALUENODE_H
