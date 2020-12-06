#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>

class McfunctionHighlighter : public QSyntaxHighlighter
{
public:
    McfunctionHighlighter(QObject *parent = nullptr);

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
    QTextCharFormat posFormat;
    QTextCharFormat entitySelectorFormat;
    QTextCharFormat namespacedIDFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat commentFormat;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
