#ifndef PARSENODE_H
#define PARSENODE_H

#include <QObject>
#include <QDebug>

namespace Command {
    class ParseNode : public QObject
    {
        Q_OBJECT
public:
        explicit ParseNode(QObject *parent, int pos = -1, int length = 0);

        virtual QString toString() const;

        int pos() const;
        void setPos(int pos);

        int length() const;
        bool isVaild() const;
protected:
        void setLength(int length);

private:
        int m_pos    = 0;
        int m_length = 0;
    };
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);

Q_DECLARE_METATYPE(Command::ParseNode*);

#endif /* PARSENODE_H */
