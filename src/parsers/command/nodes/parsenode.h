#ifndef PARSENODE_H
#define PARSENODE_H

#include "../visitors/nodevisitor.h"

#include <QDebug>

namespace Command {
    class ParseNode
    {
public:
        explicit ParseNode(int pos = -1, int length = 0);

        virtual QString toString() const;
        virtual bool isVaild() const;

        int pos() const;
        void setPos(int pos);

        int length() const;
        void setLength(int length);

        virtual void accept(NodeVisitor *visitor, NodeVisitor::Order order);

private:
        int m_pos    = 0;
        int m_length = 0;
    };
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);

Q_DECLARE_METATYPE(QSharedPointer<Command::ParseNode>);

template <typename T>
struct TypeRegister {
    static bool init() {
        qRegisterMetaType<QSharedPointer<T> >();
        QMetaType::registerConverter<QSharedPointer<T>,
                                     QSharedPointer<Command::ParseNode> >();
        return true;
    };
};

#endif /* PARSENODE_H */
