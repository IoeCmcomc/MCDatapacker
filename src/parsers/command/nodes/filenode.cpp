#include "filenode.h"

namespace Command {
    FileNode::FileNode() : ParseNode(ParseNode::Kind::Container) {
        m_isValid = true;
    }

    FileNode::Lines FileNode::lines() const {
        return m_lines;
    }
}
