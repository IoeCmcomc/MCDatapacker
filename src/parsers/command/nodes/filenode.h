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
            m_lines.push_back(std::forward<T>(node));
        }

        NodePtr const operator[](const int i) const {
            return m_lines[i];
        }

        Lines lines() const;

private:
        Lines m_lines;
    };
}

#endif // FILENODE_H
