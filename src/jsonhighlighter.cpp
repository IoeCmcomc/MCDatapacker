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
    /*qDebug() << "JsonHighlighter::highlightBlock" << text; */
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

void JsonHighlighter::checkProblems(bool) {
    /*qDebug() << "JsonHighlighter::checkProblems"; */
    auto jsonDoc = QJsonDocument::fromJson(
        getParentDoc()->toPlainText().toUtf8(), &jsonErr);

    for (auto block = getParentDoc()->begin();
         block != getParentDoc()->end(); block = block.next()) {
        if (TextBlockData *data =
                dynamic_cast<TextBlockData*>(block.userData())) {
            if (!(jsonDoc.isNull() && block.contains(jsonErr.offset))) {
                data->clearProblems();
            } else {
                QTextCursor tc(getParentDoc());
                tc.setPosition(jsonErr.offset);
                ProblemInfo error{ ProblemInfo::Type::Error,
                                   (uint)tc.blockNumber(),
                                   (uint)tc.positionInBlock(), 1,
                                   jsonErr.errorString() };
                data->setProblems({ error });
            }
        }
        /*emit document()->documentLayout()->updateBlock(block); */
    }
}
