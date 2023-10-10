#include "filenode.h"

namespace Command {
    FileNode::FileNode() : ParseNode(ParseNode::Kind::Container) {
        m_isValid = true;
    }

    FileNode::Lines FileNode::lines() const {
        return m_lines;
    }

    SourceMapper &FileNode::sourceMapper() {
        return m_srcMapper;
    }

    void FileNode::setSourceMapper(SourceMapper &&newSrcMapper) {
        m_srcMapper = newSrcMapper;
    }
}

QDebug operator<<(QDebug debug, const Command::SourceMapper::Info &value) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "Info(" << value.pos << ", " << value.trivia << ')';
    return debug;
}
