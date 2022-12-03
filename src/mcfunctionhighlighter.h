#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"
#include "parsers/command/minecraftparser.h"

class McfunctionHighlighter : public Highlighter
{
public:
    McfunctionHighlighter(QTextDocument *parent = nullptr);

protected slots:
    void highlightBlock(const QString &text) override;
    void rehighlightBlock(const QTextBlock &block,
                          const QVector<QTextLayout::FormatRange> &formats);

    void checkProblems(bool checkAll = false) override;

private:
    void setupRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;
    QVector<QTextLayout::FormatRange> m_formats;

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat posFormat;
    QTextCharFormat entitySelectorFormat;
    QTextCharFormat namespacedIDFormat;
    QTextCharFormat commentFormat;

    Command::MinecraftParser parser;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
