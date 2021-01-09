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

struct NamespacedIdInfo {
    int     start;
    int     length;
    QString link;
};

class CodeEditor;

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData() = default;

    void clear();

    QVector<BracketInfo *> brackets();
    QVector<NamespacedIdInfo *> namespacedIds();

    void insert(BracketInfo *info);
    void insert(NamespacedIdInfo *info);

private:
    QVector<BracketInfo*> m_brackets;
    QVector<NamespacedIdInfo*> m_namespaceIds;
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
    Highlighter(QTextDocument *parent);

protected:
    QMap<QChar, QTextCharFormat> quoteHighlightRules;
    QMap<QChar, QTextCharFormat> singleCommentHighlightRules;
    QVector<BracketPair> bracketPairs;

    friend class CodeEditor;

    void highlightBlock(const QString &text);

private:
    void collectNamespacedIds(const QString &text, TextBlockData *data);
    QString locateNamespacedId(QString id);
};

#endif /* HIGHLIGHTER_H */
