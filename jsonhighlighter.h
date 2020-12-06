#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>

class JsonHighlighter : public QSyntaxHighlighter
{
public:
    JsonHighlighter(QObject *parent = 0);

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
