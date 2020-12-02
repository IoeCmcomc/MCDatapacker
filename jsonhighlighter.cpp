#include "jsonhighlighter.h"

#include <QColor>
#include <QDebug>

JsonHighlighter::JsonHighlighter(QTextDocument *parent, QObject *parentObj)
    : QSyntaxHighlighter(parent) {
    this->doc       = parent;
    this->parentObj = parentObj;
    setupRules();
}

void JsonHighlighter::setupRules() {
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
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
    rule.pattern =
        QRegularExpression(QStringLiteral(
                               "-?(?:[1-9]\\d*|0)(?:\\.\\d+)?(:?[eE][+-]?\\d+)?"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    cBracketFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("[{}]"));
    rule.format  = cBracketFormat;
    highlightingRules.append(rule);

    rBracketFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("[[\\]]"));
    rule.format  = rBracketFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(QColor(170, 0, 0));
    rule.pattern =
        QRegularExpression(QStringLiteral("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
}

void JsonHighlighter::setDoc(QTextDocument *value) {
    qDebug() << "setDocument" << doc << value;
    doc = value;
    setDocument(value);
    qDebug() << doc << document();
}

void JsonHighlighter::highlightBlock(const QString &text) {
    if (this->enabled) {
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

    setCurrentBlockState(0);
}

void JsonHighlighter::setEnabled(bool state) {
    qDebug() << "JsonHighlighter::setEnabled :" << state;
    this->enabled = state;

/*
      qDebug() << doc << document();
      setDocument(doc);
      qDebug() << doc << document();
      setDocument(state ? doc : 0);
 */

    qDebug() << "setEnabled done";

    /*rehighlight(); */
}
