#ifndef FILENODE_H
#define FILENODE_H

#include "parsenode.h"

namespace Command {
    class FileNode : public ParseNode {
public:
        using Lines = std::list<NodePtr>;

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

        Lines lines() const;

    private:
        Lines m_lines;
    };
}

#endif // FILENODE_H
