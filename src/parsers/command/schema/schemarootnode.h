#ifndef SCHEMA_ROOTNODE_H
#define SCHEMA_ROOTNODE_H

#include "schemanode.h"

namespace Command::Schema {
    class RootNode : public Node {
public:
        RootNode();

protected:
        friend void from_json(const json& j, RootNode *&n);
    };

    void from_json(const json& j, RootNode *&n);
}

#endif // SCHEMA_ROOTNODE_H
