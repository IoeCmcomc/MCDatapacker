#ifndef MCFUNCTIONPARSER_H
#define MCFUNCTIONPARSER_H

#include "minecraftparser.h"
#include "nodes/filenode.h"

namespace Command {
    class McfunctionParser final : public Parser {
public:
        enum class State {
            Command,
            Comment,
            Macro,
        };

        McfunctionParser();
        ~McfunctionParser() {
        };

        QSharedPointer<FileNode> syntaxTree() const;

protected:
        bool parseImpl() final;

private:
        MinecraftParser m_commandParser;
        ParseNodeCache m_cache;
        QSharedPointer<FileNode> m_tree;
    };
}

#endif // MCFUNCTIONPARSER_H
