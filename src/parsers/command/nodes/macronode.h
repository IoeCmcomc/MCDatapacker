#ifndef MACRONODE_H
#define MACRONODE_H

#include "parsenode.h"

namespace Command {
    class MacroNode : public ParseNode {
public:
        MacroNode(const int length);

        bool isEmpty() const {
            return m_segments.isEmpty();
        }
        int size() const {
            return m_segments.size();
        }

        void append(NodePtr node);

        QVector<NodePtr> segments() const;

private:
        QVector<NodePtr> m_segments;
    };

    class MacroVariableNode : public ParseNode {
public:
        explicit MacroVariableNode(const QString &key, const int length,
                                   const bool valid = false)
            : ParseNode{Kind::MacroVariable, length}, m_key{key} {
            m_isValid = valid;
        }

private:
        QString m_key;
    };

    DECLARE_TYPE_ENUM(ParseNode::Kind, Macro)
    DECLARE_TYPE_ENUM(ParseNode::Kind, MacroVariable)
}

#endif // MACRONODE_H
