#include "schemaloader.h"

#include "schemarootnode.h"
#include "schemaliteralnode.h"

#include <QFileInfo>
#include <QDebug>


namespace Command::Schema {
    using ParserType = Schema::ArgumentNode::ParserType;

    SchemaLoader::SchemaLoader(const QString &filepath) {
        QFileInfo finfo(filepath);

        if (!(finfo.exists() && finfo.isFile())) {
            qWarning() << "File not exists:" << finfo.filePath();
            m_error = QT_TRANSLATE_NOOP("SchemaLoader", "File not exists");
            return;
        }

        QFile      f(finfo.filePath());
        const bool isJson      = finfo.suffix() == QLatin1String("json");
        const auto openOptions = (isJson)
                                     ? (QIODevice::ReadOnly |
                                        QIODevice::Text) : QIODevice::ReadOnly;

        if (!f.open(openOptions)) {
            m_error = f.errorString();
            return;
        }

        QByteArray &&data = f.readAll();
        f.close();

        json j;

        try {
            if (isJson) {
                j = json::parse(data);
            } else if (finfo.suffix() == QLatin1String("msgpack")) {
                j = json::from_msgpack(data);
            }

            m_tree = j.get<Schema::RootNode *>();
            resolveRedirects(j, m_tree);
            patchTree();
            // TODO: Merge duplicated sub-trees
        } catch (const json::exception &e) {
            qWarning() << "Loading schema failed: " << e.what();
            m_error = e.what();
        }
    }

    Schema::RootNode * SchemaLoader::tree() const {
        return m_tree;
    }

    void SchemaLoader::resolveRedirects(const json &j, Schema::Node *node) {
        Q_ASSERT(m_tree != nullptr);

        if (j.contains("redirect")) {
            // Example: "execute as >@e< at @s ..."
            node->setRedirect(
                getNodeFromJsonArr(j["redirect"].get<json::array_t>()));
        }

        if (j.contains("children")) {
            const auto &children        = j["children"];
            const auto &literalChildren = node->literalChildren();
            for (auto it = literalChildren.cbegin();
                 it != literalChildren.cend(); ++it) {
                resolveRedirects(children[it.key().toStdString()], it.value());
            }
            const auto &argChildren = node->argumentChildren();
            for (const auto &child: argChildren) {
                resolveRedirects(children[child->name().toStdString()], child);
            }
        } else if (!(node->redirect() || node->isExecutable())) {
            // Example: "execute >run< say ..."
            node->setRedirect(m_tree);
        }
    }

    Node * SchemaLoader::getNodeFromJsonArr(const json::array_t &path) const {
        Schema::Node *node = m_tree;

        QString pathStr;

        for (const auto &element: path) {
            bool            found = false;
            const QString &&key   = element.get<QString>();
            if (node->literalChildren().contains(key)) {
                node  = node->literalChildren().value(key);
                found = true;
            } else {
                const auto &&arguments = node->argumentChildren();
                for (auto *argNode: arguments) {
                    if (argNode->name() == key) {
                        node  = argNode;
                        found = true;
                        break;
                    }
                }
            }
            pathStr += '/';
            pathStr += key;
            if (!found) {
                qDebug() << "getNodeFromJsonArr: Node not found:" << pathStr;
                return nullptr;
            }
        }

        return node;
    }

    Node * SchemaLoader::getNode(const ConstCharsVec &path) const {
        Schema::Node *node = m_tree;

        QString pathStr;

        for (const char *key: path) {
            bool            found  = false;
            const QString &&keyStr = QString::fromLatin1(key);
            if (node->literalChildren().contains(keyStr)) {
                node  = node->literalChildren().value(keyStr);
                found = true;
            } else {
                const auto &&arguments = node->argumentChildren();
                for (auto *argNode: arguments) {
                    if (argNode->name() == keyStr) {
                        node  = argNode;
                        found = true;
                        break;
                    }
                }
            }
            pathStr += '/';
            pathStr += key;
            if (!found) {
                qDebug() << "getNodeFromJsonArr: Node not found:" << pathStr;
                return nullptr;
            }
        }

        return node;
    }

    void SchemaLoader::patchNodeProps(const ConstCharsVec &path,
                                      ArgumentNode::ParserType parserType,
                                      QVariantMap newProps) {
        if (auto *node = getNode(path);
            node && (node->kind() == Node::Kind::Argument)) {
            auto *argNode = static_cast<Schema::ArgumentNode *>(node);
            if (argNode->parserType() == parserType) {
                auto &&props = argNode->properties();
                props.insert(newProps);
                argNode->setProperties(props);
            }
        }
    }

    void SchemaLoader::patchAdvancementTargets(const char *subCommand) {
        const QVariantMap patchMap{ { "category", "advancement" } };

        patchNodeProps(
            { "advancement", subCommand, "targets", "from", "advancement" },
            ParserType::ResourceLocation, patchMap);
        patchNodeProps(
            { "advancement", subCommand, "targets", "only", "advancement" },
            ParserType::ResourceLocation, patchMap);
        patchNodeProps(
            { "advancement", subCommand, "targets", "through", "advancement" },
            ParserType::ResourceLocation, patchMap);
        patchNodeProps(
            { "advancement", subCommand, "targets", "until", "advancement" },
            ParserType::ResourceLocation, patchMap);
    }

    void SchemaLoader::patchItemModifiers(const char *subCommand,
                                          const char *argName) {
        const QVariantMap patchMap{ { "category", "item_modifier" } };

        patchNodeProps({ "item", "modify", subCommand, argName, "slot",
                         "modifier" },
                       ParserType::ResourceLocation, patchMap);
        patchNodeProps(
            { "item", "replace", subCommand, argName, "slot", "from", "block",
              "source", "sourceSlot", "modifier" },
            ParserType::ResourceLocation, patchMap);
        patchNodeProps(
            { "item", "replace", subCommand, argName, "slot", "from", "entity",
              "source", "sourceSlot", "modifier" },
            ParserType::ResourceLocation, patchMap);
    }

    void SchemaLoader::patchLootSources(const ConstCharsVec &sourcePath) {
        const QVariantMap patchMap{ { "category", "loot_table" } };
        ConstCharsVec     fishPath;
        ConstCharsVec     lootPath;

        fishPath << "loot" << sourcePath << "fish" << "loot_table";
        lootPath << "loot" << sourcePath << "loot" << "loot_table";

        patchNodeProps(fishPath, ParserType::ResourceLocation, patchMap);
        patchNodeProps(lootPath, ParserType::ResourceLocation, patchMap);
    }

    void SchemaLoader::patchTree() {
        // Patch nodes in the "advancement" command
        patchAdvancementTargets("grant");
        patchAdvancementTargets("revoke");

        // Patch nodes in the "bossbar" command
        QVariantMap bossbarIdPatchMap{ { "category", "bossbar" } };
        patchNodeProps({ "bossbar", "add", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);
        patchNodeProps({ "bossbar", "get", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);
        patchNodeProps({ "bossbar", "remove", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);
        patchNodeProps({ "bossbar", "set", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);

        // Patch nodes in the "execute" command
        patchNodeProps({ "execute", "if", "predicate", "predicate" },
                       ParserType::ResourceLocation,
                       { { "category", "predicate" } });
        patchNodeProps({ "execute", "unless", "predicate", "predicate" },
                       ParserType::ResourceLocation,
                       { { "category", "predicate" } });
        patchNodeProps({ "execute", "store", "result", "bossbar", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);
        patchNodeProps({ "execute", "store", "success", "bossbar", "id" },
                       ParserType::ResourceLocation, bossbarIdPatchMap);

        // Patch nodes in the "item" command
        patchItemModifiers("block", "pos");
        patchItemModifiers("entity", "targets");

        patchNodeProps({ "locatebiome", "biome" }, ParserType::ResourceLocation,
                       { { "category", "worldgen/biome" } });

        // Patch nodes in the "loot" command
        patchLootSources({ "give", "players" });
        patchLootSources({ "insert", "targetPos" });
        patchLootSources({ "spawn", "targetPos" });
        patchLootSources({ "replace", "block", "targetPos", "slot" });
        patchLootSources({ "replace", "block", "targetPos", "slot", "count" });
        patchLootSources({ "replace", "entity", "entities", "slot" });
        patchLootSources({ "replace", "entity", "entities", "slot", "count" });

        QVariantMap soundIdPatchMap{ { "category", "sound_event" } };
        patchNodeProps({ "playsound", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "*", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "ambient", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "block", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "hostile", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "master", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "music", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "neutral", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "player", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "record", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "voice", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);
        patchNodeProps({ "stopsound", "targets", "weather", "sound" },
                       ParserType::ResourceLocation, soundIdPatchMap);

        // Patch nodes in the "recipe" command
        patchNodeProps({ "recipe", "give", "targets", "recipe" },
                       ParserType::ResourceLocation,
                       { { "category", "recipe" } });
        patchNodeProps({ "recipe", "take", "targets", "recipe" },
                       ParserType::ResourceLocation,
                       { { "category", "recipe" } });
    }
}
