#ifndef RESOURCELOCATIONNODE_H
#define RESOURCELOCATIONNODE_H

#include "argumentnode.h"

namespace Command {
    class ResourceLocationNode : public ArgumentNode
    {
public:
        explicit ResourceLocationNode(int pos               = -1,
                                      const QString &nspace = "",
                                      const QString &id     = "");
        virtual QString toString() const override;
        void accept(NodeVisitor *visitor,
                    NodeVisitor::Order order = NodeVisitor::Order::Postorder)
        override;
        QString format() const;

        QString nspace() const;
        void setNspace(const QString &nspace);

        QString id() const;
        void setId(const QString &id);

        QString fullId() const;

        bool isTag() const;
        void setIsTag(bool isTag);

private:
        QString m_nspace;
        QString m_id;
        bool m_isTag = false;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::ResourceLocationNode>)

#endif /* RESOURCELOCATIONNODE_H */
