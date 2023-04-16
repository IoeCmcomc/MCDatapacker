#include "gamemodenode.h"

#include "../visitors/nodevisitor.h"

namespace Command {
    GamemodeNode::GamemodeNode(const QString &text, const Mode &value,
                               const bool isValid)
        : ArgumentNode(ParserType::Gamemode, text), m_value(value) {
        this->m_isValid = isValid;
    }

    DEFINE_ACCEPT_METHOD(GamemodeNode)
}
