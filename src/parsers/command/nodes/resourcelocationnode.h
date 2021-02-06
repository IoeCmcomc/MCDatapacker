#ifndef RESOURCELOCATIONNODE_H
#define RESOURCELOCATIONNODE_H

#include "argumentnode.h"

namespace Command {
    class ResourceLocationNode : public ArgumentNode
    {
        Q_OBJECT
public:
        explicit ResourceLocationNode(QObject *parent, int pos = -1,
                                      const QString &nspace    = "minecraft",
                                      const QString &id        = "");

        virtual QString toString() const;

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

Q_DECLARE_METATYPE(Command::ResourceLocationNode*)

#endif /* RESOURCELOCATIONNODE_H */
