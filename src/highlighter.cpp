#include "highlighter.h"

#include "globalhelpers.h"
#include "game.h"

#include "lru/lru.hpp"

#include <QDebug>
#include <QDir>
#include <QTextDocument>

// #include <chrono>

using namespace std::chrono_literals;

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
    : QSyntaxHighlighter(parent), m_quoteDelimiters{'"'},
    m_palette{defaultCodePalette} {
    connect(parent, &QTextDocument::contentsChanged,
            this, &Highlighter::onDocChanged);

    m_curDirExists = QDir::current().exists();

    bracketPairs.append({ '{', '}' });
    bracketPairs.append({ '[', ']' });
    initBracketCharset();

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

bool Highlighter::hasAdvancedHighlighting() const {
    return m_hasAdvancedHighlighting;
}

void Highlighter::ensureDelayedRehighlightAll() {
    if (m_changedBlocks.size() < document()->blockCount()) {
        m_changedBlocks.clear();
        m_changedBlocks.reserve(document()->blockCount());
        for (auto &&block = document()->begin(); block != document()->end();
             block = block.next()) {
            m_changedBlocks << block;
        }
    }
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
    const bool highlightManually = m_highlightManually;

    // qDebug() << "Highlighter::highlightBlock" << currentBlock().blockNumber() <<
    //     highlightManually;
    Q_ASSERT(document() != nullptr);

    TextBlockData *data = nullptr;

    bool hasCurData = false;
    if (!highlightManually) {
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
    } else {
        data = dynamic_cast<TextBlockData *>(currentBlockUserData());
    }

    const QStringView sv{ text };
    const QStringView trimmed = sv.trimmed();
    if ((!trimmed.isEmpty()) && trimmed[0] == m_singleCommentChar) {
        setCurrentBlockState(Comment);
        setFormat(0, sv.length(), m_palette[CodePalette::Comment]);
    } else {
        QChar curQuoteChar = '\0';
        int   quoteStart   = 0;
        int   quoteLength  = 0;
        bool  backslash    = false;
        for (int i = 0; i < sv.length(); ++i) {
            const QChar curChar = sv[i];
            if (m_quoteDelimiters.contains(curChar)) {
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
                        ++quoteLength;
                    }
                } else {
                    ++quoteLength;
                    backslash = false;
                }
            } else if (backslash) {
                ++quoteLength;
                backslash = false;
            } else if (curChar == '\\') {
                backslash = true;
                ++quoteLength;
            } else {
                switch (currentBlockState()) {
                    case QuotedString: {
                        ++quoteLength;
                        break;
                    }
                    case Normal: {
                        collectBracket(i, curChar, data);
                        break;
                    }
                    default: {
                        if (curChar.isSpace() || curChar == '\t')
                            setFormat(i, 1, m_invisSpaceFmt);
                    }
                }
            }
        }
    }
    if (currentBlockState() == QuotedString
        || currentBlockState() == Comment) {
        setCurrentBlockState(Normal);
    }

    if (m_curDirExists) {
        if (!highlightManually) {
            collectNamespacedIds(sv, data);
        }
        formatNamespacedIds(data);
    }

    if (data && !hasCurData && !highlightManually) {
        setCurrentBlockUserData(data);
    }
}

void Highlighter::highlightUsingRules(const QString &text,
                                      const HighlightingRules &rules) {
    for (const HighlightingRule &rule : rules) {
        QRegularExpressionMatchIterator matchIterator =
            rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            if (const int start = match.capturedStart(1); start != -1) {
                // Only highlight the range of the first capturing group
                mergeFormat(start, match.capturedLength(1),
                            m_palette[rule.formatRole]);
            } else {
                // Highlight the whole match
                mergeFormat(match.capturedStart(), match.capturedLength(),
                            m_palette[rule.formatRole]);
            }
        }
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

QVector<QTextBlock> &Highlighter::changedBlocks() {
    return m_changedBlocks;
}

void Highlighter::initBracketCharset() {
    m_bracketCharset.reserve(bracketPairs.size() * 2);
    for (const auto &pair: qAsConst(bracketPairs)) {
        m_bracketCharset << pair.left << pair.right;
    }
}

void Highlighter::formatNamespacedIds(TextBlockData *data,
                                      const QTextCharFormat &baseFmt) {
    if (!data)
        return;

    const auto &infos = data->namespacedIds();

    auto fmt = baseFmt;
    for (const auto info: infos) {
        fmt.setAnchor(true);
        fmt.setAnchorHref(info->link);
        fmt.setToolTip(info->link);
        fmt.setFontUnderline(true);
        mergeFormat(info->start, info->length, fmt);
    }
}

void Highlighter::setHasAdvancedHighlighting(bool newHasAdvancedHighlighting) {
    m_hasAdvancedHighlighting = newHasAdvancedHighlighting;
}

void Highlighter::collectBracket(int i, QChar ch, TextBlockData *data) {
    if (!data)
        return;

    if (m_bracketCharset.contains(ch)) {
        auto *info = new BracketInfo;
        info->character = ch.toLatin1();
        info->pos       = i;
        data->insert(info);
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
            // qDebug() << match.captured() << info->start << info->length;
            data->insert(info);
        }
    }
}

// std::once_flag flag;

/// Naive algorithm to find a file path from a namespaced ID
QString Highlighter::locateNamespacedId(QString id) {
    static LRU::TimedCache<QString, QStringList> dirListCache(200ms, 8);

    // std::call_once(flag, [](){
    //     dirListCache.hit_callback([](const auto& key, const auto& value) {
    //         qDebug() << "Hit for entry ("
    //                  << key << ", " << value << ")";
    //     });

    //     dirListCache.miss_callback([](const auto& key) {
    //         qDebug() << "Miss for " << key;
    //     });
    // });

    bool isTag = false;

    if (Glhp::removePrefix(id, "#"_QL1))
        isTag = true;
    auto      dir       = QDir::current();
    QString &&nspaceKey = id.section(":", 0, 0);

    if (!dir.cd(QStringLiteral("data/") + nspaceKey)) {
        return QString();
    }
    if (isTag) {
        if (!dir.cd(QStringLiteral("tags"))) {
            return QString();
        }
        nspaceKey += QStringLiteral("/tags");
    }

    QStringList dirList;
    if (dirListCache.contains(nspaceKey)) {
        dirList = dirListCache.lookup(nspaceKey);
    } else {
        dirList = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        dirListCache.emplace(nspaceKey, dirList);
    }
    for (const auto &catDir: qAsConst(dirList)) {
        const QString &&path = dir.path() + "/"_QL1 + catDir + "/"
                               + id.section(":", 1, 1);
        if (catDir == QStringLiteral("functions")
            && QFile::exists(path + QStringLiteral(".mcfunction"))) {
            return path + QStringLiteral(".mcfunction");
        } else if (QFile::exists(path + QStringLiteral(".json"))) {
            return path + QStringLiteral(".json");
        }
    }
    if (nspaceKey == QStringLiteral("minecraft") || nspaceKey.isEmpty()) {
        const static QVector<QString> vanillaCats{
            "advancements", "loot_tables", "recipes",
        };
        const QString &idPath = id.section(':', 1, 1) + ".json";
        for (const QString &vanillaCat: vanillaCats) {
            if (Game::isVanillaFileExists(vanillaCat, idPath)) {
                return Game::realVanillaFilePath(vanillaCat, idPath);
            }
        }
    }
    return QString();
}
