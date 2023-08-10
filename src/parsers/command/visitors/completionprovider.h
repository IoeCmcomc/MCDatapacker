#ifndef COMPLETIONPROVIDER_H
#define COMPLETIONPROVIDER_H

#include "overloadnodevisitor.h"

namespace Command {

    class CompletionProvider : public OverloadNodeVisitor {
public:
        explicit CompletionProvider(const int row);

        void visit(RootNode *node) final;

        void visit(GamemodeNode *node) final;
        void visit(ResourceNode *node) final;

        QVector<QString> suggestions() const;

    private:
        QVector<QString> m_suggestions;
        int m_pos       = 0;
        int m_cursorRow = 0;
    };
}

#endif // COMPLETIONPROVIDER_H
