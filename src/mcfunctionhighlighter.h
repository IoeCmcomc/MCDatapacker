#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"

namespace Command {
    class McfunctionParser;
}

class McfunctionHighlighter : public Highlighter {
public:
    using FormatRanges = QVector<QVector<QTextLayout::FormatRange> >;

    explicit McfunctionHighlighter(QTextDocument *parent,
                                   Command::McfunctionParser *parser = nullptr);

protected slots:
    void highlightBlock(const QString &text) override;
    void rehighlightChangedBlocks();

    void rehighlightDelayed() override;

private:
    void setupRules();

    QVector<HighlightingRule> highlightingRules;
    FormatRanges m_formats;

    Command::McfunctionParser *m_parser = nullptr;
    int m_curChangedBlockIndex          = 0;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
