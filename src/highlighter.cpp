#include "highlighter.h"

#include "globalhelpers.h"

#include <QDebug>
#include <QDir>
#include <QTextDocument>

TextBlockData::~TextBlockData() {
    clear();
}

void TextBlockData::clear() {
    qDeleteAll(m_brackets);
    m_brackets.clear();
    qDeleteAll(m_namespaceIds);
    m_namespaceIds.clear();
}

QVector<BracketInfo *> TextBlockData::brackets() {
    return m_brackets;
}

QVector<NamespacedIdInfo *> TextBlockData::namespacedIds() {
    return m_namespaceIds;
}


void TextBlockData::insert(BracketInfo *info) {
    m_brackets << info;
}

void TextBlockData::insert(NamespacedIdInfo *info) {
    m_namespaceIds << info;
}


Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), m_palette{defaultCodePalette} {
    connect(parent, &QTextDocument::contentsChanged,
            this, &Highlighter::onDocChanged);

    m_curDirExists = QDir::current().exists();

    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor(QStringLiteral("#A31621")));

    quoteHighlightRules.insert('"', fmt);

    bracketPairs.append({ '{', '}' });
    bracketPairs.append({ '[', ']' });

    m_invisSpaceFmt.setForeground(Qt::lightGray);

/*
      connect(parent, &QTextDocument::contentsChanged,
              this, &Highlighter::onDocChanged);
 */
}

void Highlighter::rehighlight() {
    m_highlightManually = true;
    QSyntaxHighlighter::rehighlight();
    m_highlightManually = false;
}

void Highlighter::rehighlightBlock(const QTextBlock &block) {
    m_highlightManually = true;
    QSyntaxHighlighter::rehighlightBlock(block);
    m_highlightManually = false;
}

bool Highlighter::isManualHighlight() const {
    return m_highlightManually;
}

void Highlighter::setPalette(const CodePalette &newPalette) {
    m_palette = newPalette;
}

void Highlighter::onDocChanged() {
    //qDebug() << "Highlighter::onDocChanged";
    m_highlightingFirstBlock = true;
}

QVector<QTextBlock> Highlighter::changedBlocks() const {
    return m_changedBlocks;
}

void Highlighter::highlightBlock(const QString &text) {
//    qDebug() << "Highlighter::highlightBlock" << currentBlock().blockNumber();
    Q_ASSERT(document() != nullptr);

    TextBlockData *data = nullptr;

    bool hasCurData = false;
    if (!m_highlightManually) {
        if (m_highlightingFirstBlock) {
            m_changedBlocks.clear();
            m_highlightingFirstBlock = false;
        }
        m_changedBlocks << currentBlock();

        hasCurData = currentBlockUserData();
        if (hasCurData) {
            data = dynamic_cast<TextBlockData *>(currentBlockUserData());
            Q_ASSERT(data != nullptr);
            data->clear();
        } else {
            data = new TextBlockData();
        }
    }

    const QStringView sv{ text };
    if ((!sv.isEmpty()) &&
        singleCommentHighlightRules.contains(sv[0])) {
        setCurrentBlockState(Comment);
        setFormat(0, sv.length(), m_palette[CodePalette::Comment]);
    } else {
        QChar curQuoteChar = '\0';
        int   quoteStart   = 0;
        int   quoteLength  = 0;
        bool  backslash    = false;
        for (int i = 0; i < sv.length(); i++) {
            auto curChar = sv[i];
            if (quoteHighlightRules.contains(curChar)) {
                if (!backslash) {
                    if (curChar == curQuoteChar) {
                        setCurrentBlockState(Normal);
                        setFormat(quoteStart, quoteLength + 1,
                                  m_palette[CodePalette::QuotedString]);
                        curQuoteChar = '\0';
                        quoteLength  = 0;
                    } else if (currentBlockState() == Normal) {
                        setCurrentBlockState(QuotedString);
                        curQuoteChar = curChar;
                        quoteStart   = i;
                        quoteLength++;
                    }
                } else {
                    quoteLength++;
                    backslash = false;
                }
            } else if (backslash) {
                quoteLength++;
                backslash = false;
            } else if (curChar == '\\') {
                backslash = true;
                quoteLength++;
            } else if (currentBlockState() == QuotedString) {
                quoteLength++;
            } else if (currentBlockState() == Normal) {
                collectBracket(i, curChar, data);
            }
            if (curChar.isSpace() || curChar == '\t')
                setFormat(i, 1, m_invisSpaceFmt);
        }
    }
    if (currentBlockState() == QuotedString
        || currentBlockState() == Comment) {
        setCurrentBlockState(Normal);
    }

    if (m_curDirExists)
        collectNamespacedIds(sv, data);

    if (data && !hasCurData) {
        setCurrentBlockUserData(data);
    }
}

void Highlighter::mergeFormat(int start, int count,
                              const QTextCharFormat &fmt) {
    for (int i = 0; i < count; ++i) {
        QTextCharFormat &&newFmt = format(start + i);
        newFmt.merge(fmt);
        setFormat(start + i, 1, newFmt);
    }
}

void Highlighter::collectBracket(int i, QChar ch, TextBlockData *data) {
    if (!data)
        return;

    for (const auto &bracketPair: qAsConst(bracketPairs)) {
        if (ch == bracketPair.left ||
            ch == bracketPair.right) {
            auto *info = new BracketInfo;
            info->character = ch.toLatin1();
            info->pos       = i;
            data->insert(info);
        }
    }
}

void Highlighter::collectNamespacedIds(QStringView sv, TextBlockData *data) {
    if (!data)
        return;

    auto &&matchIter = namespacedIdRegex.globalMatch(sv);

    while (matchIter.hasNext()) {
        auto            match    = matchIter.next();
        const QString &&filepath = locateNamespacedId(match.captured());
        if (!filepath.isEmpty()) {
            auto *info = new NamespacedIdInfo();
            info->start  = match.capturedStart();
            info->length = match.capturedLength();
            info->link   = filepath;
            /*qDebug() << match.captured() << info->start << info->length; */
            data->insert(info);
        }
    }
}

QString Highlighter::locateNamespacedId(QString id) {
    bool isTag = false;

    if (Glhp::removePrefix(id, "#"_QL1))
        isTag = true;
    auto dir = QDir::current();
    dir.cd(QStringLiteral("data/") + id.section(":", 0, 0));
    if (isTag)
        dir.cd(QStringLiteral("tags"));

    if (!dir.exists()) return QString();

    const QStringList &&dirList =
        dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const auto &catDir: dirList) {
        const QString &&path = dir.path() + "/"_QL1 + catDir + "/"
                               + id.section(":", 1, 1);
        if (catDir == QStringLiteral("functions")
            && QFile::exists(path + QStringLiteral(".mcfunction"))) {
            return path + QStringLiteral(".mcfunction");
        } else if (QFile::exists(path + QStringLiteral(".json"))) {
            return path + QStringLiteral(".json");
        }
    }
    return QString();
}
