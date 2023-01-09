#ifndef MCFUNCTIONPARSER_H
#define MCFUNCTIONPARSER_H

#include "minecraftparser.h"
#include "nodes/filenode.h"

namespace Command {
    class McfunctionParser : public Parser
    {
public:
        McfunctionParser();

        void parse();

        QSharedPointer<FileNode> syntaxTree() const;

        QHash<int, Errors> errorsByLine() const;

private:
        MinecraftParser m_commandParser;
        QHash<int, Errors> m_errorsByLine;
        QSharedPointer<FileNode> m_tree;
    };
}

#endif // MCFUNCTIONPARSER_H
