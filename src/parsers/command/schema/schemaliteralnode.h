#ifndef SCHEMA_LITERALNODE_H
#define SCHEMA_LITERALNODE_H

#include "schemanode.h"

namespace Command::Schema {
    class ArgumentNode;

    class LiteralNode : public Node {
public:
        LiteralNode();

protected:
        friend void from_json(const json &j, LiteralNode *&n);
    };

    void from_json(const json &j, LiteralNode *&n);
}

#endif // SCHEMA_LITERALNODE_H
