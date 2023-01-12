#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"

namespace Command {
    class McfunctionParser;
}

class McfunctionHighlighter : public Highlighter {
public:
    McfunctionHighlighter(QTextDocument *parent,
                          Command::McfunctionParser *parser = nullptr);

protected slots:
    void highlightBlock(const QString &text) override;
    void rehighlightBlock(const QTextBlock &block,
                          const QVector<QTextLayout::FormatRange> &formats);

    void rehighlightDelayed() override;

private:
    void setupRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat entitySelectorFormat;
    QTextCharFormat namespacedIDFormat;
    QTextCharFormat commentFormat;

    QVector<HighlightingRule> highlightingRules;
    QVector<QTextLayout::FormatRange> m_formats;

    Command::McfunctionParser *m_parser = nullptr;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
