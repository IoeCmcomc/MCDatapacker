#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

#include <optional>

struct BracketInfo {
    int  position;
    char character;
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

struct ProblemInfo {
    int     start;
    int     length;
    QString message;
    bool    isError   = false;
    bool    isWarning = false;

    ProblemInfo(bool isError = false) {
        this->isError   = isError;
        this->isWarning = !isError;
    }
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

    std::optional<ProblemInfo> problem() const;
    void setProblem(const std::optional<ProblemInfo> &problem);

private:
    QVector<BracketInfo*> m_brackets;
    QVector<NamespacedIdInfo*> m_namespaceIds;
    std::optional<ProblemInfo> m_problem;
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

    QTextDocument *getParentDoc() const;

    virtual void checkProblems() = 0;

public:
    using QSyntaxHighlighter::rehighlightBlock;
    void rehighlightBlock(const QTextBlock &block);

protected:
    QMap<QChar, QTextCharFormat> quoteHighlightRules;
    QMap<QChar, QTextCharFormat> singleCommentHighlightRules;
    QVector<BracketPair> bracketPairs;

    friend class CodeEditor;

    void highlightBlock(const QString &text);
    void mergeFormat(int start, int count, const QTextCharFormat &fmt);

private:
    QVector<QTextBlock> m_changedBlocks;
    bool m_highlightMunually             = false;
    QRegularExpression namespacedIdRegex =
        QRegularExpression(QStringLiteral(
                               R"(#?\b[a-z0-9-_.]+:[a-z0-9-_.\/]+\b)"));

    QTextDocument *m_parentDoc;

    void collectNamespacedIds(const QString &text, TextBlockData *data);
    QString locateNamespacedId(QString id);
};

#endif /* HIGHLIGHTER_H */
