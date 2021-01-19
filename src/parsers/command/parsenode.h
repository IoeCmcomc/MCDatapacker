#ifndef PARSENODE_H
#define PARSENODE_H

#include <QObject>

namespace CommandParser {
    class ParseNode : public QObject
    {
        Q_OBJECT
public:
        explicit ParseNode(QObject *parent = nullptr);

        QString toString() const;
    };
}

QDebug operator<<(QDebug debug, const CommandParser::ParseNode &node);


#endif /* PARSENODE_H */
