#ifndef MCBUILDHIGHLIGHTER_H
#define MCBUILDHIGHLIGHTER_H

#include "highlighter.h"

class McbuildHighlighter : public Highlighter {
    Q_OBJECT
public:
    McbuildHighlighter(QTextDocument *parent, const bool isMacroFile);

protected:
    void highlightBlock(const QString &text) final;

private:
    HighlightingRules m_rules;
};

#endif // MCBUILDHIGHLIGHTER_H
