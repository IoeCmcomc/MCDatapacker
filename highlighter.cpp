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

    fmt.setForeground(QColor("#A31621"));

    quoteHighlightRules.insert('"', fmt);
}

void Highlighter::highlightBlock(const QString &text) {
    qDebug() << "Highlighter::highlightBlock" << text;
    if (document()) {
        TextBlockData *data = new TextBlockData;

        if ((!text.isEmpty()) &&
            singleCommentHighlightRules.contains(text[0])) {
            setCurrentBlockState(Comment);
            setFormat(0, text.length(), singleCommentHighlightRules[text[0]]);
        } else {
            QChar curQuoteChar = '\0';
            int   quoteStart   = 0;
            int   quoteLength  = 0;
            bool  backslash    = false;
            for (int i = 0; i < text.length(); i++) {
                auto curChar = text[i];
                if (quoteHighlightRules.contains(curChar)) {
                    if (!backslash && (currentBlockState() == QuotedString)) {
                        setCurrentBlockState(Normal);
                        setFormat(quoteStart, quoteLength + 1,
                                  quoteHighlightRules[curQuoteChar]);
                    } else {
                        setCurrentBlockState(QuotedString);
                        curQuoteChar = curChar;
                        quoteStart   = i;
                        quoteLength++;
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
                        quoteLength++;
                    }
                }
/*
                  qDebug() << i << curChar << currentBlockState() <<
                      (currentBlockState() <= Normal);
 */
            }
        }

        setCurrentBlockUserData(data);
    }
}
