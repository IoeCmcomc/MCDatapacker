#include "jsonhighlighter.h"

#include <QDebug>

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : Highlighter(parent) {
    setupRules();
}

void JsonHighlighter::setupRules() {
    const QRegularExpression keywordPatterns("\\b(?:true|false|null)\\b");

    highlightingRules.append({ keywordPatterns, CodePalette::Keyword });

    highlightingRules.append({
        QRegularExpression(
            R"(\b-?(?:[1-9]\d*|0)(?:\.\d+)?(:?[eE][+-]?\d+)?\b)"),
        CodePalette::Number });
}

void JsonHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
            QRegularExpressionMatchIterator matchIterator =
                rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(),
                          match.capturedLength(),
                          m_palette[rule.formatRole]);
            }
        }
    }
}
