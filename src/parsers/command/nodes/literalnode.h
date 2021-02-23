#ifndef LITERALNODE_H
#define LITERALNODE_H

#include "parsenode.h"

namespace Command {
    class LiteralNode : public ParseNode
    {
        Q_OBJECT
public:
        explicit LiteralNode(QObject *parent, int pos, const QString &txt);

        QString toString() const;
        QString text() const;
        void setText(const QString &text);

private:
        QString m_text;
    };
}

Q_DECLARE_METATYPE(Command::LiteralNode*);

#endif /* LITERALNODE_H */
