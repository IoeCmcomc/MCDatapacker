#ifndef BOOLNODE_H
#define BOOLNODE_H

#include "argumentnode.h"

namespace Command {
    class BoolNode : public ArgumentNode
    {
public:
        explicit BoolNode(int pos = -1, bool value = false);

        QString toString() const override;
        void accept(NodeVisitor *visitor) override {
            visitor->visit(this);
        }
        bool value() const;
        void setValue(bool value);

private:
        bool m_value = false;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::BoolNode>);

#endif /* BOOLNODE_H */
