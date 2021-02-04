#ifndef BOOLNODE_H
#define BOOLNODE_H

#include "argumentnode.h"

namespace Command {
    class BoolNode : public ArgumentNode
    {
        Q_OBJECT
public:
        explicit BoolNode(QObject *parent, int pos = -1, bool value = false);

        QString toString() const;
        bool value() const;
        void setValue(bool value);

private:
        bool m_value;
    };
}

Q_DECLARE_METATYPE(Command::BoolNode*);

#endif /* BOOLNODE_H */
