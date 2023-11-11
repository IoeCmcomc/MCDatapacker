#include "jsonhighlighter.h"

#include <QRegularExpression>

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : Highlighter(parent) {
    setupRules();
}

void JsonHighlighter::setupRules() {
    highlightingRules.append({
        QRegularExpression("\\b(?:true|false|null)\\b"),
        CodePalette::Keyword });
    highlightingRules.append({
        QRegularExpression(
            R"(\b-?(?:[1-9]\d*|0)(?:\.\d+)?(:?[eE][+-]?\d+)?\b)"),
        CodePalette::Number });
}

void JsonHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        highlightUsingRules(text, highlightingRules);
    }
}
