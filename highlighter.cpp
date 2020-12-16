#include "highlighter.h"

#include "mainwindow.h"
#include "globalhelpers.h"

#include <QDebug>
#include <QDir>
#include <QRegularExpression>

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


Highlighter::Highlighter(QObject *parent) : QSyntaxHighlighter(parent) {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor("#A31621"));

    quoteHighlightRules.insert('"', fmt);

    bracketPairs.append({ '{', '}' });
    bracketPairs.append({ '[', ']' });
}

void Highlighter::highlightBlock(const QString &text) {
    /*qDebug() << "Highlighter::highlightBlock" << text; */
    if (document()) {
        auto *data = new TextBlockData;

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
                } else if (curChar == QLatin1Char('\\')) {
                    backslash = !backslash;
                } else if (currentBlockState() == QuotedString) {
                    quoteLength++;
                } else if (currentBlockState() <= Normal) {
                    for (auto &bracketPair: bracketPairs) {
                        if (curChar == bracketPair.left ||
                            curChar == bracketPair.right) {
                            auto *info = new BracketInfo;
                            info->character = curChar.toLatin1();
                            info->position  = i;
                            /*qDebug() << info->character << info->position; */
                            data->insert(info);
                        }
                    }
                }
/*
                  qDebug() << i << curChar << currentBlockState() <<
                      (currentBlockState() <= Normal);
 */
            }
        }

        collectNamespacedIds(text, data);

        setCurrentBlockUserData(data);
    }
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
    Q_ASSERT(!MainWindow::getCurDir().isEmpty());
    bool isTag = false;
    if (Glhp::removePrefix(id, "#"))
        isTag = true;
    QString dirpath = MainWindow::getCurDir() + "/data/"
                      + id.section(":", 0, 0);
    if (isTag)
        dirpath += "/tags";

    QDir dir(dirpath);

    if (!dir.exists()) return "";

    QStringList dirList =
        dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    QString path;
    for (const auto &catDir: dirList) {
        /*qDebug() << "catDir: " << catDir; */
        path = dir.path() + "/" + catDir + "/" + id.section(":", 1, 1);
        if (catDir == "functions" && QFile::exists(path + ".mcfunction")) {
            return path + ".mcfunction";
        } else if (QFile::exists(path + ".json")) {
            return path + ".json";
        }
    }

    return "";
}
