#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include "highlighter.h"

class JsonHighlighter : public Highlighter {
public:
    explicit JsonHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) final;

private:
    HighlightingRules highlightingRules;

    void setupRules();
};

#endif /* JSONHIGHLIGHTER_H */
