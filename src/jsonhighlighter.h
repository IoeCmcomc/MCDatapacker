#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QRegularExpression>
#include <QTextDocument>

#include "highlighter.h"

class JsonHighlighter : public Highlighter
{
public:
    JsonHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    void setupRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat    format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat cBracketFormat;
    QTextCharFormat rBracketFormat;
    QTextCharFormat quotationFormat;
};

#endif /* JSONHIGHLIGHTER_H */
