#include "schemaloader.h"

#include "schemarootnode.h"
#include "schemaargumentnode.h"
#include "schemaliteralnode.h"

#include <QFileInfo>
#include <QDebug>


namespace Command::Schema {
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
                m_tree->literalChildren().value(j["redirect"][0].get<QString>()));
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
}
