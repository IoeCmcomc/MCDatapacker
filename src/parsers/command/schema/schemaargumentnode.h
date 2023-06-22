#ifndef SCHEMA_ARGUMENTNODE_H
#define SCHEMA_ARGUMENTNODE_H

#include "schemanode.h"
#include "../nodes/argumentnode.h"

#include <QVariant>

namespace Command::Schema {
    class ArgumentNode : public Node {
public:
        using ParserType = Command::ArgumentNode::ParserType;

        ArgumentNode();

        void setName(const QString &newName);

        QString name() const;
        QVariantMap properties() const;
        ParserType parserType() const;

private:
        QString m_name;
        QVariantMap m_props;
        ParserType m_parserType;

        friend void from_json(const json &j, ArgumentNode *&n);
    };

    void from_json(const json &j, ArgumentNode *&n);
}

#endif // SCHEMA_ARGUMENTNODE_H
