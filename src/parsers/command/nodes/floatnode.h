#ifndef FLOATNODE_H
#define FLOATNODE_H

#include "argumentnode.h"

namespace Command {
    class FloatNode : public ArgumentNode
    {
        Q_OBJECT
public:
        explicit FloatNode(QObject *parent, int pos = -1, int length = 0,
                           float value              = false);

        QString toString() const;
        float value() const;
        void setValue(float value);

private:
        float m_value;
    };
}

Q_DECLARE_METATYPE(Command::FloatNode*);

#endif /* FLOATNODE_H */
