#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

struct ParenthesisInfo {
    char character;
    int  position;
};

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData();

    QVector<ParenthesisInfo *> parentheses();
    void insert(ParenthesisInfo *info);

private:
    QVector<ParenthesisInfo *> m_parentheses;
};

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum BlockState {
        Normal,
        QuotedString,
        Comment
    };
    Highlighter(QObject *parent);

protected:
    QMap<QChar, QTextCharFormat> quoteHighlightRules;
    QMap<QChar, QTextCharFormat> singleCommentHighlightRules;

    void highlightBlock(const QString &text);
};

#endif /* HIGHLIGHTER_H */
