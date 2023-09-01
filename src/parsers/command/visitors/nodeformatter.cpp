#include "nodeformatter.h"

#include <QRandomGenerator>

namespace Command {
    NodeFormatter::NodeFormatter(const CodePalette &palette) :
        OverloadNodeVisitor(LetTheVisitorDecide), m_palette{palette} {
    }

    QVector<QTextLayout::FormatRange> NodeFormatter::formatRanges() const {
        return m_formatRanges;
    }

    void NodeFormatter::reset() {
        m_pos = 0;
        m_formatRanges.clear();
    }

    QTextCharFormat NodeFormatter::defaultFormat(ParseNode *node) const {
        QTextCharFormat fmt;

//        fmt.setBackground(QColor(QRandomGenerator::global()->bounded(127, 255),
//                                 QRandomGenerator::global()->bounded(127, 255),
//                                 QRandomGenerator::global()->bounded(127, 255),
//                                 220));

        return fmt;
    }
}
