#include "nodeformatter.h"

namespace Command {
    NodeFormatter::NodeFormatter() : NodeVisitor(LetTheVisitorDecide) {
        m_bgfmt.setBackground(QColor(19, 232, 157, 200));
        m_bgfmt.setFontUnderline(true);
        m_bgfmt.setUnderlineColor(QColor(Qt::GlobalColor::magenta));
    }

    QVector<QTextLayout::FormatRange> NodeFormatter::formatRanges()
    const {
        return m_formatRanges;
    }
}
