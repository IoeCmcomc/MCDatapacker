#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "argumentnode.h"
#include "literalnode.h"

namespace Command {
    class StringNode : public ArgumentNode
    {
public:
        explicit StringNode(int pos              = -1,
                            const QString &value = "",
                            bool isQuote         = false);

        virtual QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }
        QString value() const;
        void setValue(const QString &value);

        static QSharedPointer<StringNode> fromLiteralNode(LiteralNode *node);

private:
        QString m_value;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::StringNode>);

#endif /* STRINGNODE_H */
