#ifndef PARSENODE_H
#define PARSENODE_H

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

private:
        int m_pos    = 0;
        int m_length = 0;
    };
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);

Q_DECLARE_METATYPE(QSharedPointer<Command::ParseNode>);

#endif /* PARSENODE_H */
