
#ifndef GAMEMODENODE_H
#define GAMEMODENODE_H

#include "argumentnode.h"

namespace Command {
    class GamemodeNode : public ArgumentNode {
public:
        enum class Mode: char {
            Unknown = -1,
            Survival,
            Creative,
            Adventure,
            Spectator,
        };

        GamemodeNode(const QString &text, const Mode &value,
                     const bool isValid = false);

        void accept(NodeVisitor *visitor, VisitOrder) final;

        inline Mode value() const {
            return m_value;
        }

        inline void setValue(const Mode &newValue) {
            m_value = newValue;
        }

protected:
        Mode m_value{};
    };

    DECLARE_TYPE_ENUM(ArgumentNode::ParserType, Gamemode)

    template<>
    constexpr std::array staticSuggestions<GamemodeNode> = {
        "adventure"_QL1, "creative"_QL1, "spectator"_QL1, "survival"_QL1,
    };
}
#endif // GAMEMODENODE_H
