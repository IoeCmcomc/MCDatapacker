#ifndef LITERALNODE_H
#define LITERALNODE_H

#include "parsenode.h"

namespace Command {
    class LiteralNode : public ParseNode
    {
public:
        explicit LiteralNode(QObject *parent, const QString &txt);

        QString toString() const;
        QString text() const;
        void setText(const QString &text);

private:
        QString m_text;
    };
}

#endif /* LITERALNODE_H */
