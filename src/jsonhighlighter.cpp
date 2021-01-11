#include "jsonhighlighter.h"

#include <QColor>
#include <QDebug>


JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : Highlighter(parent) {
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
}

void JsonHighlighter::highlightBlock(const QString &text) {
    /*qDebug() << "JsonHighlighter::highlightBlock"; */
    Highlighter::highlightBlock(text);
    if (this->document()) {
        if (jsonErr.error == QJsonParseError::NoError) {
            /*qDebug() << "Highlight normally"; */
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

    setCurrentBlockState(0);
}

void JsonHighlighter::checkProblems() {
    /*qDebug() << "JsonHighlighter::checkProblems"; */
    auto jsonDoc = QJsonDocument::fromJson(
        getParentDoc()->toPlainText().toUtf8(), &jsonErr);

    for (auto block = getParentDoc()->begin();
         block != getParentDoc()->end(); block = block.next()) {
        if (TextBlockData *data =
                dynamic_cast<TextBlockData*>(block.userData())) {
            if (!(jsonDoc.isNull() && block.contains(jsonErr.offset))) {
                data->setProblem(std::nullopt);
            } else {
                auto error = ProblemInfo(true);
                error.start   = jsonErr.offset;
                error.length  = 1;
                error.message = jsonErr.errorString();

                qDebug() << "A problem found at line"
                         << block.blockNumber()
                         << ", row"
                         << error.start - block.position()
                         << ":" << error.message;

                data->setProblem(error);
            }
        }
    }
}
