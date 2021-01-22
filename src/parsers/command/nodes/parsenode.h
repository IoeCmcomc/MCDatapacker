#ifndef PARSENODE_H
#define PARSENODE_H

#include <QObject>
#include <QDebug>

namespace Command {
    class ParseNode : public QObject
    {
public:
        explicit ParseNode(QObject *parent = nullptr);

        virtual QString toString() const;
    };
}

QDebug operator<<(QDebug debug, const Command::ParseNode &node);

#endif /* PARSENODE_H */
