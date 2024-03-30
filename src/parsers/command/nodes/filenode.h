#ifndef FILENODE_H
#define FILENODE_H

#include "parsenode.h"

#include <QMap>

namespace Command {
    struct SourceMapper {
        struct Info {
            int     pos;
            QString trivia;
        };

        QMap<int, Info> backslashMap;      // Map logical positions to physical backslash positions
        QMap<int, int>  physicalPositions; // Map logical positions to physical positions
        QMap<int, int>  logicalPositions;  // Map physical positions to logical positions
        QVector<int>    logicalLines;      // Indexes are logical line numbers, values are their physical line numbers

        static int binarySearchIndexOf(const QVector<int> &container,
                                       const int value) {
            const auto &&it =
                std::lower_bound(container.cbegin(), container.cend(), value);

            if (it != container.cend() && *it == value) {
                return it - container.cbegin();
            }
            return -1;
        }

        int logicalLinesIndexOf(const int pos) const {
            return binarySearchIndexOf(logicalLines, pos);
        }
    };

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

        SourceMapper &sourceMapper();
        void setSourceMapper(SourceMapper &&newSrcMapper);

private:
        SourceMapper m_srcMapper;
        Lines m_lines;
    };
}

QDebug operator<<(QDebug debug, const Command::SourceMapper::Info &value);

#endif // FILENODE_H
