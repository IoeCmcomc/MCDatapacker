#ifndef JMCHIGHLIGHTER_H
#define JMCHIGHLIGHTER_H

#include "highlighter.h"

class JmcHighlighter : public Highlighter {
    Q_OBJECT
public:
    JmcHighlighter(QTextDocument *parent, const bool isHeaderFile);

protected:
    void highlightBlock(const QString &text) final;

private:
    HighlightingRules m_rules;
};

#endif // JMCHIGHLIGHTER_H
