#include "highlighter.h"

#include <QDebug>

TextBlockData::TextBlockData() {
}

QVector<ParenthesisInfo *> TextBlockData::parentheses() {
    return m_parentheses;
}


void TextBlockData::insert(ParenthesisInfo *info) {
    int i = 0;

    while (i < m_parentheses.size() &&
           info->position > m_parentheses.at(i)->position)
        ++i;

    m_parentheses.insert(i, info);
}


Highlighter::Highlighter(QObject *parent) : QSyntaxHighlighter(parent) {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor(170, 0, 0));

    quoteHighlightRules.insert('"', fmt);
}

void Highlighter::highlightBlock(const QString &text) {
    qDebug() << "Highlighter::highlightBlock" << text;
    if (document()) {
        TextBlockData *data = new TextBlockData;

        QChar curQuoteChar = '\0';
        bool  backslash    = false;
        for (int i = 0; i < text.length(); i++) {
            auto curChar = text[i];
            if (quoteHighlightRules.contains(curChar)) {
                if (!backslash && (currentBlockState() == QuotedString)) {
                    setCurrentBlockState(Normal);
                } else {
                    setCurrentBlockState(QuotedString);
                    curQuoteChar = curChar;
                }
            } else if (curChar == '\\') {
                backslash = !backslash;
            } else if (curChar == '{') {
                if (currentBlockState() <= Normal) {
                    ParenthesisInfo *info = new ParenthesisInfo;
                    info->character = '{';
                    info->position  = i;

                    qDebug() << info->character << info->position;
                    data->insert(info);
                }
            } else if (curChar == '}') {
                if (currentBlockState() <= Normal) {
                    ParenthesisInfo *info = new ParenthesisInfo;
                    info->character = '}';
                    info->position  = i;

                    qDebug() << info->character << info->position;
                    data->insert(info);
                }
            } else {
                if (currentBlockState() == QuotedString) {
                    setFormat(i, 1, quoteHighlightRules[curQuoteChar]);
                }
            }
            qDebug() << i << curChar << currentBlockState() <<
                (currentBlockState() <= Normal);
        }

        setCurrentBlockUserData(data);
    }
}
