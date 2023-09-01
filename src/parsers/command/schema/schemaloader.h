#ifndef SCHEMALOADER_H
#define SCHEMALOADER_H

#include <QString>

#include "nlohmann/json.hpp"

#include <memory>

using json = nlohmann::json;

namespace Command::Schema {
    class Node;
    class RootNode;

    class SchemaLoader {
public:
        SchemaLoader(const QString &filepath);

        QString lastError() const {
            return m_error;
        };

        Schema::RootNode * tree() const;

private:
        QString m_error;
        Schema::RootNode *m_tree = nullptr;

        void resolveRedirects(const json &j, Node *node);
    };
}
#endif // SCHEMALOADER_H
