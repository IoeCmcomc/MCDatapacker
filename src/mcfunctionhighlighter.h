#ifndef MCFUNCTIONHIGHLIGHTER_H
#define MCFUNCTIONHIGHLIGHTER_H

#include <QRegularExpression>

#include "highlighter.h"
#include "parsers/command/minecraftparser.h"

class McfunctionHighlighter : public Highlighter
{
public:
    McfunctionHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;
    void checkProblems() override;

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

    Command::MinecraftParser parser;
};

#endif /* MCFUNCTIONHIGHLIGHTER_H */
