#include "highlighter.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QDir>
#include <QRegularExpression>

void TextBlockData::clear() {
    qDeleteAll(m_brackets);
    m_brackets.clear();
    qDeleteAll(m_namespaceIds);
    m_namespaceIds.clear();
    m_problem = std::nullopt;
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
           info->position > m_brackets.at(i)->position)
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

std::optional<ProblemInfo> TextBlockData::problem() const {
    return m_problem;
}

void TextBlockData::setProblem(const std::optional<ProblemInfo> &problem) {
    m_problem = problem;
}

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    m_parentDoc = parent;
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor(QStringLiteral("#A31621")));

    quoteHighlightRules.insert('"', fmt);

    bracketPairs.append({ '{', '}' });
    bracketPairs.append({ '[', ']' });
}

void Highlighter::highlightBlock(const QString &text) {
    /*qDebug() << "Highlighter::highlightBlock" << text; */
    if (document()) {
        TextBlockData *data;
        if (currentBlockUserData()) {
            data =
                dynamic_cast<TextBlockData*>(currentBlockUserData());
            data->clear();
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
                        setFormat(quoteStart, quoteLength + 2,
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
                } else if (currentBlockState() == QuotedString) {
                    quoteLength++;
                } else if (currentBlockState() <= Normal) {
                    for (auto &bracketPair: bracketPairs) {
                        if (curChar == bracketPair.left ||
                            curChar == bracketPair.right) {
                            auto *info = new BracketInfo;
                            info->character = curChar.toLatin1();
                            info->position  = i;
                            data->insert(info);
                        }
                    }
                }
            }
        }

        collectNamespacedIds(text, data);

        if (!currentBlockUserData()) {
            setCurrentBlockUserData(data);
        }
    }
}

QTextDocument *Highlighter::getParentDoc() const {
    return m_parentDoc;
}

void Highlighter::collectNamespacedIds(const QString &text,
                                       TextBlockData *data) {
    QRegularExpression namespacedIdRegex =
        QRegularExpression(QStringLiteral("#?\\b[a-z0-9-_.]+:[a-z0-9-_./]+\\b"));
    auto matchIter = namespacedIdRegex.globalMatch(text);

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
