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


class TextBlockData : public QTextBlockUserData {
public:
    TextBlockData() = default;
    ~TextBlockData();

    void clear();

    QVector<BracketInfo *> brackets();
    QVector<NamespacedIdInfo *> namespacedIds();

    void insert(BracketInfo *info);
    void insert(NamespacedIdInfo *info);

private:
    QVector<BracketInfo *> m_brackets;
    QVector<NamespacedIdInfo *> m_namespaceIds;
};


class CodeEditor;
class Parser;

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    enum BlockState {
        Normal = -1,
        QuotedString,
        Comment,
        MultilineComment,
    };

    explicit Highlighter(QTextDocument *parent);

    using QSyntaxHighlighter::rehighlightBlock;

    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);

    QVector<QTextBlock> changedBlocks() const;
    void onDocChanged();

    bool isManualHighlight() const;

protected:
    QHash<QChar, QTextCharFormat> quoteHighlightRules;
    QHash<QChar, QTextCharFormat> singleCommentHighlightRules;
    QVector<BracketPair> bracketPairs;
    QRegularExpression namespacedIdRegex{ QStringLiteral(
                                              R"(#?\b[a-z0-9-_.]+:[a-z0-9-_.\/]+)") };

    friend class CodeEditor;

    void highlightBlock(const QString &text);
    void mergeFormat(int start, int count, const QTextCharFormat &fmt);
    virtual void rehighlightDelayed() {
    };

private:
    QVector<QTextBlock> m_changedBlocks;
    QTextCharFormat m_invisSpaceFmt;
    bool m_highlightManually      = false;
    bool m_highlightingFirstBlock = false;
    bool m_curDirExists           = false;

    void collectBracket(int i, QChar ch, TextBlockData *data);
    void collectNamespacedIds(QStringView sv, TextBlockData *data);
    QString locateNamespacedId(QString id);
};

#endif /* HIGHLIGHTER_H */
