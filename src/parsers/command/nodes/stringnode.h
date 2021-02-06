#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "argumentnode.h"

namespace Command {
    class StringNode : public ArgumentNode
    {
        Q_OBJECT
public:
        explicit StringNode(QObject *parent, int pos = -1,
                            const QString &value     = "");

        virtual QString toString() const;
        QString value() const;
        void setValue(const QString &value);

private:
        QString m_value;
    };
}

Q_DECLARE_METATYPE(Command::StringNode*);

#endif /* STRINGNODE_H */
