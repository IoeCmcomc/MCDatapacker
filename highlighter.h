#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

struct BracketInfo {
    char character;
    int  position;
};

struct BracketPair {
    char left;
    char right;
};

class CodeEditor;

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData();

    QVector<BracketInfo *> brackets();
    void insert(BracketInfo *info);

private:
    QVector<BracketInfo *> m_brackets;
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
    QVector<BracketPair> bracketPairs;

    friend class CodeEditor;

    void highlightBlock(const QString &text);
};

#endif /* HIGHLIGHTER_H */
