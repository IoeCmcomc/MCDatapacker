#ifndef FILENODE_H
#define FILENODE_H

#include "parsenode.h"

namespace Command {
    class FileNode : public ParseNode {
public:
        using Lines = QVector<NodePtr>;

        FileNode();

        bool isEmpty() const {
            return m_lines.empty();
        }
        int size() const {
            return m_lines.size();
        }
        template<class T>
        void append(T&& node) {
            m_isValid = m_lines.empty() ? node->isValid()
                                        : m_isValid && node->isValid();
            m_lines.push_back(std::forward<T>(node));
        }

        NodePtr const at(const int i) const {
            return m_lines.at(i);
        }

        Lines lines() const;

private:
        Lines m_lines;
    };
}

#endif // FILENODE_H
