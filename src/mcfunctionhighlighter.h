#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"

namespace Command {
    class McfunctionParser;
}

class McfunctionHighlighter : public Highlighter {
public:
    using FormatRanges = QVector<QTextLayout::FormatRange>;

    explicit McfunctionHighlighter(QTextDocument *parent,
                                   Command::McfunctionParser *parser = nullptr);

protected slots:
    void highlightBlock(const QString &text) final;
    void rehighlightChangedBlocks();

    void rehighlightDelayed() final;

private:
    QVector<HighlightingRule> highlightingRules;
    QVector<FormatRanges> m_formats;

    Command::McfunctionParser *m_parser = nullptr;
    int m_curChangedBlockIndex          = 0;

    void setupRules();
    static QVector<FormatRanges> splitRangesToLines(
        const FormatRanges &ranges, QVector<int> &breakPositions,
        const int offset = 0);
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
