#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include "highlighter.h"

#include <QRegularExpression>

class JsonHighlighter : public Highlighter {
public:
    explicit JsonHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) override;

private:
    QVector<HighlightingRule> highlightingRules;

    void setupRules();
};

#endif /* JSONHIGHLIGHTER_H */
