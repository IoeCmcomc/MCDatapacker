#ifndef MULTIMAPNODE_H
#define MULTIMAPNODE_H

#include "mapnode.h"

namespace Command {
    using ParseNodeMultiMap = QMultiMap<MapKey, QSharedPointer<ParseNode> >;

    class MultiMapNode : public ParseNode
    {
public:
        MultiMapNode(int pos, int length = 0);

        QString toString() const override;
        void accept(NodeVisitor *visitor, NodeVisitor::Order order) override;

        int size() const;
        bool contains(const MapKey &key) const;
        void insert(const MapKey &key, QSharedPointer<ParseNode> node);
        void replace(const MapKey &key, QSharedPointer<ParseNode> node);
        int remove(const MapKey &key);
        void clear();
        QList<QSharedPointer<ParseNode> > values(const MapKey &key) const;
        ParseNodeMultiMap toMap() const;

private:
        ParseNodeMultiMap m_map;
    };
}

Q_DECLARE_METATYPE(QSharedPointer<Command::MultiMapNode>)
#endif /* MULTIMAPNODE_H */
