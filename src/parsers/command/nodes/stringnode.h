#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "argumentnode.h"

namespace Command {
    class StringNode : public ArgumentNode
    {
public:
        explicit StringNode(QObject *parent, int pos = -1,
                            const QString &value     = "");

        QString toString() const;
        QString value() const;
        void setValue(const QString &value);

private:
        QString m_value;
    };
}

#endif /* STRINGNODE_H */
