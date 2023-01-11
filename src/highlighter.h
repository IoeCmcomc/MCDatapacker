#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include <optional>

struct BracketInfo {
    int  pos       = 0;
    char character = '\0';
};


struct BracketPair {
    char left  = '\0';
    char right = '\0';
};


struct NamespacedIdInfo {
    int     start  = 0;
    int     length = 1;
    QString link;
};


struct ProblemInfo {
    enum class Type {
        Invaild,
        Warning,
        Error,
    };

    Type    type   = Type::Invaild;
    int     line   = 0;
    int     col    = 0;
    int     length = 1;
    QString message;
};


class TextBlockData : public QTextBlockUserData {
public:
    TextBlockData() = default;
    ~TextBlockData();

    void clear();
    void clearProblems();

    QVector<BracketInfo *> brackets();
    QVector<NamespacedIdInfo *> namespacedIds();

    void insert(BracketInfo *info);
    void insert(NamespacedIdInfo *info);

    QVector<ProblemInfo> problems() const;
    void setProblems(const QVector<ProblemInfo> &problems);

private:
    QVector<ProblemInfo> m_problems;
    QVector<BracketInfo *> m_brackets;
    QVector<NamespacedIdInfo *> m_namespaceIds;
};


class CodeEditor;


class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    enum BlockState {
        Normal,
        QuotedString,
        Comment,
        MultilineComment,
    };
    Highlighter(QTextDocument *parent);

    QTextDocument * getParentDoc() const;

    virtual void checkProblems(bool checkAll = false) = 0;

public:
    using QSyntaxHighlighter::rehighlightBlock;
    void rehighlightBlock(const QTextBlock &block);

    QVector<QTextBlock> changedBlocks() const;
    void onDocChanged();


protected:
    QMap<QChar, QTextCharFormat> quoteHighlightRules;
    QMap<QChar, QTextCharFormat> singleCommentHighlightRules;
    QVector<BracketPair> bracketPairs;
    QRegularExpression namespacedIdRegex{ QStringLiteral(
                                              R"(#?\b[a-z0-9-_.]+:[a-z0-9-_.\/]+)") };

    friend class CodeEditor;

    void highlightBlock(const QString &text);
    void mergeFormat(int start, int count, const QTextCharFormat &fmt);

private:
    QVector<QTextBlock> m_changedBlocks;
    bool m_highlightMunually      = false;
    bool m_highlightingFirstBlock = false;


    QTextDocument *m_parentDoc;
    QTextCharFormat m_invisSpaceFmt;

    void collectNamespacedIds(const QString &text, TextBlockData *data);
    QString locateNamespacedId(QString id);
};

#endif /* HIGHLIGHTER_H */
