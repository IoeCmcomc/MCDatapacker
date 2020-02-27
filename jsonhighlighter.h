#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>

class JsonHighlighter : public QSyntaxHighlighter
{
public:
    JsonHighlighter(QTextDocument *parent = 0, QObject *parentObj = nullptr);
    void setEnabled(bool state);

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

    bool enabled       = false;
    QTextDocument *doc = nullptr;
    QObject *parentObj;
};

#endif // JSONHIGHLIGHTER_H
