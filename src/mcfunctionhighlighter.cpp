#include "mcfunctionhighlighter.h"
#include "codeeditor.h"

#include "parsers/command/visitors/nodeformatter.h"

#include <QDebug>
#include <QElapsedTimer>

McfunctionHighlighter::McfunctionHighlighter(QTextDocument *parent)
    : Highlighter(parent), parser(this) {
    setupRules();
}

void McfunctionHighlighter::setupRules() {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor(62, 195, 0));
    singleCommentHighlightRules.insert('#', fmt);

    fmt = QTextCharFormat();

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    keywordFormat.setToolTip("minecraft command");
    const QString keywordPatterns[] = {
        QStringLiteral("\\badvancement\\b"),
        QStringLiteral("\\bban\\b"),
        QStringLiteral("\\bban-ip\\b"),
        QStringLiteral("\\bbanlist\\b"),
        QStringLiteral("\\bbossbar\\b"),
        QStringLiteral("\\bclear\\b"),
        QStringLiteral("\\bclone\\b"),
        QStringLiteral("\\bdata\\b"),
        QStringLiteral("\\bdatapack\\b"),
        QStringLiteral("\\bdebug\\b"),
        QStringLiteral("\\bdefaultgamemode\\b"),
        QStringLiteral("\\bdeop\\b"),
        QStringLiteral("\\bdifficulty\\b"),
        QStringLiteral("\\beffect\\b"),
        QStringLiteral("\\bechant\\b"),
        QStringLiteral("\\bexecute\\b"),
        QStringLiteral("\\bexperience\\b"),
        QStringLiteral("\\bfill\\b"),
        QStringLiteral("\\bforceload\\b"),
        QStringLiteral("\\bfunction\\b"),
        QStringLiteral("\\bgamemode\\b"),
        QStringLiteral("\\bgamerule\\b"),
        QStringLiteral("\\bgive\\b"),
        QStringLiteral("\\bhelp\\b"),
        QStringLiteral("\\bkick\\b"),
        QStringLiteral("\\bkill\\b"),
        QStringLiteral("\\blist\\b"),
        QStringLiteral("\\blocate\\b"),
        QStringLiteral("\\bloot\\b"),
        QStringLiteral("\\bme\\b"),
        QStringLiteral("\\bmsg\\b"),
        QStringLiteral("\\bop\\b"),
        QStringLiteral("\\bpardon\\b"),
        QStringLiteral("\\bparticle\\b"),
        QStringLiteral("\\blpaysound\\b"),
        QStringLiteral("\\bpublish\\b"),
        QStringLiteral("\\brecipe\\b"),
        QStringLiteral("\\breload\\b"),
        QStringLiteral("\\breplaceitem\\b"),
        QStringLiteral("\\bsave-all\\b"),
        QStringLiteral("\\bsave-off\\b"),
        QStringLiteral("\\bsave-on\\b"),
        QStringLiteral("\\bsay\\b"),
        QStringLiteral("\\bschedule\\b"),
        QStringLiteral("\\bscoreboard\\b"),
        QStringLiteral("\\bseed\\b"),
        QStringLiteral("\\bsetblock\\b"),
        QStringLiteral("\\bsetidletimeout\\b"),
        QStringLiteral("\\bsetworldspawn\\b"),
        QStringLiteral("\\bspawnpoint\\b"),
        QStringLiteral("\\bspectate\\b"),
        QStringLiteral("\\bspreadplayer\\b"),
        QStringLiteral("\\bstop\\b"),
        QStringLiteral("\\bstopsound\\b"),
        QStringLiteral("\\bsummon\\b"),
        QStringLiteral("\\btag\\b"),
        QStringLiteral("\\bteam\\b"),
        QStringLiteral("\\bban\\b"),
        QStringLiteral("\\bteleport\\b"),
        QStringLiteral("\\bteammsg\\b"),
        QStringLiteral("\\btell\\b"),
        QStringLiteral("\\btellraw\\b"),
        QStringLiteral("\\btime\\b"),
        QStringLiteral("\\btitle\\b"),
        QStringLiteral("\\btp\\b"),
        QStringLiteral("\\btrigger\\b"),
        QStringLiteral("\\bw\\b"),
        QStringLiteral("\\bweather\\b"),
        QStringLiteral("\\bwhitelist\\b"),
        QStringLiteral("\\bworldborder\\b"),
        QStringLiteral("\\bxp\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format  = keywordFormat;
        highlightingRules.append(rule);
    }

    numberFormat.setForeground(QColor(47, 151, 193));
    rule.pattern =
        QRegularExpression(QStringLiteral(
                               R"((?<!\w)-?\d+(?:\.\d+)?[bBsSlLfFdD]?(?!\w))"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    posFormat.setForeground(QColor(171, 129, 205));
    rule.pattern = QRegularExpression(QStringLiteral("[~^]?-?\\d+(?:\\s|$)"));
    rule.format  = posFormat;
    highlightingRules.append(rule);

    namespacedIDFormat.setForeground(QColor(69, 80, 59));
    rule.pattern =
        QRegularExpression(QStringLiteral(R"(\b[a-z0-9-_]+:[a-z0-9-_/.]+\b)"));
    rule.format = namespacedIDFormat;
    highlightingRules.append(rule);

    auto quoteFmt = QTextCharFormat();
    quoteFmt.setForeground(QColor(163, 22, 33));

    quoteHighlightRules.insert('\'', quoteFmt);

    commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("^#.*"));
    rule.format  = commentFormat;
    highlightingRules.append(rule);
}

void McfunctionHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        auto *data = static_cast<TextBlockData*>(currentBlockUserData());

        if (!data)
            return;

        const auto &infos = data->namespacedIds();

        for (const auto info: infos) {
            auto fmt = namespacedIDFormat;
            fmt.setFontUnderline(true);
            fmt.setToolTip(info->link);
            setFormat(info->start, info->length, fmt);
        }

        if (m_formats.isEmpty()) {
            for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
                QRegularExpressionMatchIterator &&matchIterator =
                    rule.pattern.globalMatch(text);
                while (matchIterator.hasNext()) {
                    QRegularExpressionMatch &&match = matchIterator.next();
                    setFormat(match.capturedStart(), match.capturedLength(),
                              rule.format);
                }
            }
        } else {
            for (const auto &range: qAsConst(m_formats)) {
                mergeFormat(range.start, range.length, range.format);
            }
        }
    }

    setCurrentBlockState(0);
}

void McfunctionHighlighter::rehighlightBlock(const QTextBlock &block,
                                             const QVector<QTextLayout::FormatRange> formats)
{
    m_formats = formats;
    Highlighter::rehighlightBlock(block);
    m_formats.clear();
}

void McfunctionHighlighter::checkProblems(bool checkAll) {
    QSharedPointer<Command::ParseNode> result = nullptr;

    QElapsedTimer timer;

    timer.start();
    const auto &&changedBlks = changedBlocks();
    const auto  *doc         = document();
    Q_ASSERT(doc != nullptr);

    auto block =
        (!checkAll) ? ((!changedBlks.isEmpty()) ? changedBlks.constFirst() : doc
                       ->
                       findBlock(-1)) : doc->firstBlock();
    const auto &end =
        (!checkAll) ? ((!changedBlks.isEmpty()) ? changedBlks.constLast() : doc
                       ->
                       findBlock(-1)) : doc->lastBlock();

    while (block.isValid() && block.blockNumber() <= end.blockNumber()) {
        if (TextBlockData *data =
                dynamic_cast<TextBlockData*>(block.userData())) {
            QString lineText = block.text();
            parser.setText(lineText);
            QElapsedTimer timer;
            timer.start();
            result = parser.parse();
            if (result->isVaild()) {
                data->clearProblems();

                Command::NodeFormatter formatter;
                formatter.startVisiting(result.get());
                /*qDebug() << "rehighlight manually" << block.firstLineNumber(); */
                document()->blockSignals(true);
                rehighlightBlock(block, formatter.formatRanges());
                document()->blockSignals(false);

/*
                  qDebug() << "Size:" << parser.cache().size() << '/' <<
                      parser.cache().capacity()
                           << "Total access:" <<
                      parser.cache().stats().total_accesses()
                           << "Total hit:" << parser.cache().stats().total_hits()
                           << "Total miss:" << parser.cache().stats().total_hits()
                           << "Hit rate:" << parser.cache().stats().hit_rate()
                           << "Time elapsed:" << timer.elapsed();
 */
            } else {
                auto      &&parseErr = parser.lastError();
                ProblemInfo error{ ProblemInfo::Type::Error,
                                   (uint)block.blockNumber(),
                                   (uint)parseErr.pos,
                                   (uint)parseErr.length,
                                   parseErr.toLocalizedMessage(),
                };
                data->setProblems({ error });
            }
        }
        block = block.next();
    }
    qDebug() << "Size:" << parser.cache().size() << '/' <<
        parser.cache().capacity()
             << "Total access:" << parser.cache().stats().total_accesses()
             << "Total hit:" << parser.cache().stats().total_hits()
             << "Total miss:" << parser.cache().stats().total_misses()
             << "Hit rate:" << parser.cache().stats().hit_rate()
             << "Time elapsed:" << timer.elapsed();
}
