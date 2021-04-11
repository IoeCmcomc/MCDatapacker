#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include "highlighter.h"

#include <QRegularExpression>
#include <QTextDocument>
#include <QJsonDocument>

class JsonHighlighter : public Highlighter
{
public:
    JsonHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;
    void checkProblems(bool checkAll = false) override;

private:
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

    QJsonParseError jsonErr;

    void setupRules();
};

#endif /* JSONHIGHLIGHTER_H */
