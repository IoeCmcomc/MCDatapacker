#include "highlighter.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QDir>

void TextBlockData::clear() {
    qDeleteAll(m_brackets);
    m_brackets.clear();
    qDeleteAll(m_namespaceIds);
    m_namespaceIds.clear();
    m_problems.clear();
}

void TextBlockData::clearProblems() {
    m_problems.clear();
}

QVector<BracketInfo *> TextBlockData::brackets() {
    return m_brackets;
}

QVector<NamespacedIdInfo *> TextBlockData::namespacedIds() {
    return m_namespaceIds;
}


void TextBlockData::insert(BracketInfo *info) {
    int i = 0;

    while (i < m_brackets.size() &&
           info->pos > m_brackets.at(i)->pos)
        ++i;

    m_brackets.insert(i, info);
}

void TextBlockData::insert(NamespacedIdInfo *info) {
    int i = 0;

    while (i < m_namespaceIds.size() &&
           info->start > m_namespaceIds.at(i)->start)
        ++i;

    m_namespaceIds.insert(i, info);
}

QVector<ProblemInfo> TextBlockData::problems() const {
    return m_problems;
}

void TextBlockData::setProblems(const QVector<ProblemInfo> &problems) {
    m_problems = problems;
}

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    m_parentDoc = parent;
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

void Highlighter::highlightBlock(const QString &text) {
    /*qDebug() << "Highlighter::highlightBlock" << text << currentBlockState(); */
    if (m_highlightMunually) {
    } else {
        if (m_highlightingFirstBlock) {
            m_changedBlocks.clear();
            m_highlightingFirstBlock = false;
        }
        m_changedBlocks << currentBlock();
    }

    if (document()) {
        TextBlockData *data                = nullptr;
        bool           dataAlereadyExisted = false;
        if (currentBlockUserData()) {
            data =
                dynamic_cast<TextBlockData*>(currentBlockUserData());
            data->clear();
            dataAlereadyExisted = true;
        } else {
            data = new TextBlockData();
        }

        if ((!text.isEmpty()) &&
            singleCommentHighlightRules.contains(text[0])) {
            setCurrentBlockState(Comment);
            setFormat(0, text.length(), singleCommentHighlightRules[text[0]]);
        } else {
            QChar curQuoteChar = '\0';
            int   quoteStart   = 0;
            int   quoteLength  = 0;
            bool  backslash    = false;
            for (int i = 0; i < text.length(); i++) {
                auto curChar = text[i];
                if (quoteHighlightRules.contains(curChar)) {
                    if (!backslash && (currentBlockState() == QuotedString)) {
                        setCurrentBlockState(Normal);
                        setFormat(quoteStart,
                                  quoteLength + 1,
                                  quoteHighlightRules[curQuoteChar]);
                    } else {
                        setCurrentBlockState(QuotedString);
                        curQuoteChar = curChar;
                        quoteStart   = i;
                        quoteLength++;
                    }
                } else if (backslash) {
                    quoteLength++;
                    backslash = false;
                } else if (curChar == QLatin1Char('\\')) {
                    backslash = true;
                    quoteLength++;
                } else if (currentBlockState() == QuotedString) {
                    quoteLength++;
                } else if (currentBlockState() <= Normal) {
                    for (auto &bracketPair: bracketPairs) {
                        if (curChar == bracketPair.left ||
                            curChar == bracketPair.right) {
                            auto *info = new BracketInfo;
                            info->character = curChar.toLatin1();
                            info->pos       = i;
                            data->insert(info);
                        }
                    }
                }
                if (curChar.isSpace() || curChar == '\t')
                    setFormat(i, 1, m_invisSpaceFmt);
            }
        }
        if (currentBlockState() == QuotedString
            || currentBlockState() == Comment) {
            setCurrentBlockState(-1);
        }

        collectNamespacedIds(text, data);

        if (!currentBlockUserData()) {
            setCurrentBlockUserData(data);
        } else if (!dataAlereadyExisted) {
            delete data;
        }
    }
}

void Highlighter::mergeFormat(int start, int count,
                              const QTextCharFormat &fmt) {
    for (int i = 0; i < count; ++i) {
        QTextCharFormat newFmt = format(start + i);
        newFmt.merge(fmt);
        setFormat(start + i, 1, newFmt);
    }
}

void Highlighter::onDocChanged() {
    /*qDebug() << "onDocChanged"; */
    m_highlightingFirstBlock = true;
}

QVector<QTextBlock> Highlighter::changedBlocks() const {
    return m_changedBlocks;
}

void Highlighter::rehighlightBlock(const QTextBlock &block) {
    /*qDebug() << "reHighlightblock" << block.blockNumber(); */
    m_highlightMunually = true;
    QSyntaxHighlighter::rehighlightBlock(block);
    m_highlightMunually = false;
}

QTextDocument *Highlighter::getParentDoc() const {
    return m_parentDoc;
}

void Highlighter::collectNamespacedIds(const QString &text,
                                       TextBlockData *data) {
    auto &&matchIter = namespacedIdRegex.globalMatch(text);

    while (matchIter.hasNext()) {
        auto match    = matchIter.next();
        auto filepath = locateNamespacedId(match.captured());
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
    Q_ASSERT(QDir::current().exists());
    bool isTag = false;
    if (Glhp::removePrefix(id, QStringLiteral("#")))
        isTag = true;
    auto dir = QDir::current();
    dir.cd(QStringLiteral("data/") + id.section(":", 0, 0));
    if (isTag)
        dir.cd(QStringLiteral("tags"));

    if (!dir.exists()) return "";

    QStringList dirList =
        dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QString path;
    for (const auto &catDir: dirList) {
        path = dir.path() + "/" + catDir + "/" + id.section(":", 1, 1);
        if (catDir == QStringLiteral("functions")
            && QFile::exists(path + QStringLiteral(".mcfunction"))) {
            return path + QStringLiteral(".mcfunction");
        } else if (QFile::exists(path + QStringLiteral(".json"))) {
            return path + QStringLiteral(".json");
        }
    }
    return "";
}
