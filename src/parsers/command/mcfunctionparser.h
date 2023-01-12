#ifndef MCFUNCTIONPARSER_H
#define MCFUNCTIONPARSER_H

#include "minecraftparser.h"
#include "nodes/filenode.h"

namespace Command {
    class McfunctionParser : public Parser {
public:
        McfunctionParser();

        QSharedPointer<FileNode> syntaxTree() const;

protected:
        bool parseImpl() override;

private:
        MinecraftParser m_commandParser;
        QSharedPointer<FileNode> m_tree;
    };
}

#endif // MCFUNCTIONPARSER_H
