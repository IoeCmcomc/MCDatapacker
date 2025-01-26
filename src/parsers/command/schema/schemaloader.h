#ifndef SCHEMALOADER_H
#define SCHEMALOADER_H

#include "schemaargumentnode.h"

#include "nlohmann/json.hpp"

#include <QString>

// #include <memory>

using json = nlohmann::json;

namespace Command::Schema {
    class Node;
    class RootNode;

    using ConstCharsVec = QVector<const char *>;

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

        Schema::Node * getNodeFromJsonArr(const json::array_t &path) const;
        Schema::Node * getNode(const ConstCharsVec &path) const;
        void patchNodeProps(const ConstCharsVec &path,
                            Schema::ArgumentNode::ParserType parserType,
                            QVariantMap newProps);
        void patchAdvancementTargets(const char *subCommand);
        void patchItemModifiers(const char *subCommand,
                                const char *argName);
        void patchLootSources(const ConstCharsVec &sourcePath);
        void patchTree();
    };
}
#endif // SCHEMALOADER_H
