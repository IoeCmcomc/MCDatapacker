#ifndef SCHEMA_ARGUMENTNODE_H
#define SCHEMA_ARGUMENTNODE_H

#include "schemanode.h"

#include <QVariant>

namespace Command::Schema {
    class ArgumentNode : public Node {
public:
        ArgumentNode();

        void setName(const QString &newName);

        QString name() const;
        QVariantMap properties() const;
        QString parserId() const;

private:
        QString m_name;
        QString m_parserId;
        QVariantMap m_props;

        friend void from_json(const json &j, ArgumentNode *&n);
    };

    void from_json(const json &j, ArgumentNode *&n);
}

#endif // SCHEMA_ARGUMENTNODE_H
