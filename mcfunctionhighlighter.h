#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>

class MCfunctionHighlighter : public QSyntaxHighlighter
{
public:
    MCfunctionHighlighter(QTextDocument *parent = 0, QObject *parentObj = nullptr);
    void setEnabled(bool state);

protected:
    void highlightBlock(const QString &text) override;

private:
    void setupRules();

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat posFormat;
    QTextCharFormat entitySelectorFormat;
    QTextCharFormat minecraftNamespacedIDFormat;
    QTextCharFormat namespacedIDFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat commentFormat;

    bool enabled = false;
    QTextDocument *doc = nullptr;
    QObject *parentObj;
};

#endif // MCFUNCTIONHIGHLIGHTER_H
