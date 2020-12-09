#include "highlighter.h"

#include <QDebug>

TextBlockData::TextBlockData() {
}

QVector<BracketInfo *> TextBlockData::brackets() {
    return m_brackets;
}


void TextBlockData::insert(BracketInfo *info) {
    int i = 0;

    while (i < m_brackets.size() &&
           info->position > m_brackets.at(i)->position)
        ++i;

    m_brackets.insert(i, info);
}


Highlighter::Highlighter(QObject *parent) : QSyntaxHighlighter(parent) {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor("#A31621"));

    quoteHighlightRules.insert('"', fmt);

    fmt = QTextCharFormat();
    bracketPairs.append({ '{', '}' });
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
                        setFormat(quoteStart, quoteLength + 2,
                                  quoteHighlightRules[curQuoteChar]);
                    } else {
                        setCurrentBlockState(QuotedString);
                        curQuoteChar = curChar;
                        quoteStart   = i;
                        quoteLength++;
                    }
                } else if (curChar == QLatin1Char('\\')) {
                    backslash = !backslash;
                } else if (currentBlockState() == QuotedString) {
                    quoteLength++;
                } else if (currentBlockState() <= Normal) {
                    for (auto iter = bracketPairs.begin();
                         iter != bracketPairs.end();
                         ++iter) {
                        if (curChar == iter->left || curChar == iter->right) {
                            BracketInfo *info = new BracketInfo;
                            info->character = curChar.toLatin1();
                            info->position  = i;

                            qDebug() << info->character << info->position;
                            data->insert(info);
                        }
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
