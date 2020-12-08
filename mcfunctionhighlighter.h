#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"

class McfunctionHighlighter : public Highlighter
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
    QTextCharFormat commentFormat;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
