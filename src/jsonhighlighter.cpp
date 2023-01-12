#include "jsonhighlighter.h"

#include <QColor>
#include <QDebug>
#include <QAbstractTextDocumentLayout>


JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : Highlighter(parent) {
    setupRules();
}

void JsonHighlighter::setupRules() {
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
/*    keywordFormat.setToolTip("boolean"); */
    const QString keywordPatterns[] = {
        QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\bnull\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    numberFormat.setForeground(QColor(220, 150, 30));
/*    numberFormat.setToolTip("number"); */
    rule.pattern =
        QRegularExpression(R"(\b-?(?:[1-9]\d*|0)(?:\.\d+)?(:?[eE][+-]?\d+)?\b)");
    rule.format = numberFormat;
    highlightingRules.append(rule);
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
                          rule.format);
            }
        }
    }
}
