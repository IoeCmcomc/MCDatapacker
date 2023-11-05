#ifndef MCFUNCTIONPARSER_H
#define MCFUNCTIONPARSER_H

#include "minecraftparser.h"
#include "nodes/filenode.h"

namespace Command {
    class MacroNode;

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

        QSharedPointer<MacroNode> parseMacroLine(const QString &line,
                                                 const int linePos);
    };
}

#endif // MCFUNCTIONPARSER_H
