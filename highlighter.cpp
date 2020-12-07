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
}

void Highlighter::highlightBlock(const QString &text) {
    qDebug() << "Highlighter::highlightBlock" << text;
    if (document()) {
        TextBlockData *data = new TextBlockData;

        int leftPos = text.indexOf('{');

        while (leftPos != -1) {
            ParenthesisInfo *info = new ParenthesisInfo;
            info->character = '{';
            info->position  = leftPos;

            data->insert(info);
            qDebug() << info->character << info->position;

            leftPos = text.indexOf('{', leftPos + 1);
        }

        int rightPos = text.indexOf('}');
        while (rightPos != -1) {
            ParenthesisInfo *info = new ParenthesisInfo;
            info->character = '}';
            info->position  = rightPos;

            data->insert(info);
            qDebug() << info->character << info->position;

            rightPos = text.indexOf('}', rightPos + 1);
        }

        setCurrentBlockUserData(data);
    }
}
