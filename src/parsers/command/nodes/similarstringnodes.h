#ifndef SIMILARSTRINGNODES_H
#define SIMILARSTRINGNODES_H

#include "stringnode.h"

/*
   Template classes are not supported by Q_OBJECT, so I had to
   use this macro instead.
 */
#define DECLARE_TYPE_FROM_STRINGNODE(Type)       \
    class Type : public StringNode {             \
        Q_OBJECT                                 \
public:                                          \
        explicit Type(QObject * parent, int pos, \
                      const QString &txt);       \
        QString toString() const;                \
    };                                           \
/* End of macro
 */

namespace Command {
    DECLARE_TYPE_FROM_STRINGNODE(ColorNode)
    DECLARE_TYPE_FROM_STRINGNODE(EntityAnchorNode)
    DECLARE_TYPE_FROM_STRINGNODE(OperationNode)
}

Q_DECLARE_METATYPE(Command::ColorNode*);
Q_DECLARE_METATYPE(Command::EntityAnchorNode*);
Q_DECLARE_METATYPE(Command::OperationNode*);


#endif /* SIMILARSTRINGNODES_H */
