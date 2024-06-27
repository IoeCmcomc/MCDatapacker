#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "codepalette.h"

#include <QSyntaxHighlighter>
#include <QRegularExpression>


struct BracketInfo {
    int  pos       = 0;
    char character = '\0';
};
static_assert(qIsRelocatable<BracketInfo>());

struct BracketPair {
    char left  = '\0';
    char right = '\0';
};
static_assert(qIsRelocatable<BracketPair>());

struct NamespacedIdInfo {
    int     start  = 0;
    int     length = 1;
    QString link;
};
// static_assert(qIsRelocatable<NamespacedIdInfo>() == false);
// Q_DECLARE_TYPEINFO(NamespacedIdInfo, Q_RELOCATABLE_TYPE);


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
        MultilineString,
        Comment,
        MultilineComment,
        _end_BlockState,
    };

    struct HighlightingRule {
        QRegularExpression pattern;
        CodePalette::Role  formatRole;
    };
    static_assert(qIsRelocatable<HighlightingRule>() == false);

    using HighlightingRules = QVector<HighlightingRule>;

    explicit Highlighter(QTextDocument *parent);

    using QSyntaxHighlighter::rehighlightBlock;

    void rehighlight();
    void rehighlightBlock(const QTextBlock &block);

    QVector<QTextBlock> changedBlocks() const;
    void onDocChanged();

    bool isManualHighlight() const;
    bool hasAdvancedHighlighting() const;
    void ensureDelayedRehighlightAll();

    void setPalette(const CodePalette &newPalette);

    friend class CodeEditor;

protected:
    QSet<QChar> m_quoteDelimiters;
    QChar m_singleCommentChar;
    QVector<BracketPair> bracketPairs;
    QSet<QChar> m_bracketCharset;
    QRegularExpression namespacedIdRegex{ QStringLiteral(
                                              R"(#?\b[a-z0-9-_.]+:[a-z0-9-_.\/]+)") };
    CodePalette m_palette;

    void highlightBlock(const QString &text) override;
    void highlightUsingRules(const QString &text,
                             const HighlightingRules &rules);
    void mergeFormat(int start, int count, const QTextCharFormat &fmt);
    void setHasAdvancedHighlighting(bool newHasAdvancedHighlighting);
    QVector<QTextBlock> &changedBlocks();
    virtual void rehighlightDelayed() {
    };
    void initBracketCharset();
    void formatNamespacedIds(TextBlockData *data,
                             const QTextCharFormat &baseFmt = {});

private:
    QVector<QTextBlock> m_changedBlocks;
    QTextCharFormat m_invisSpaceFmt;
    bool m_highlightManually       = false;
    bool m_highlightingFirstBlock  = false;
    bool m_curDirExists            = false;
    bool m_hasAdvancedHighlighting = false;

    void collectBracket(int i, QChar ch, TextBlockData *data);
    void collectNamespacedIds(QStringView sv, TextBlockData *data);
    QString locateNamespacedId(QString id);
};

Q_DECLARE_TYPEINFO(Highlighter::HighlightingRule, Q_RELOCATABLE_TYPE);

#endif /* HIGHLIGHTER_H */
