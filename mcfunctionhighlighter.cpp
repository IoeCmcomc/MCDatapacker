#include "mcfunctionhighlighter.h"
#include "codeeditor.h"

#include <QDebug>

McfunctionHighlighter::McfunctionHighlighter(QObject *parent)
    : Highlighter(parent) {
    setupRules();
}

void McfunctionHighlighter::setupRules() {
    auto fmt = QTextCharFormat();

    fmt.setForeground(QColor("#3EC300"));
    singleCommentHighlightRules.insert('#', fmt);

    fmt = QTextCharFormat();

    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
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

    numberFormat.setForeground(QColor("#2F97C1"));
    rule.pattern =
        QRegularExpression(QStringLiteral(
                               "(?<!\\w)-?\\d+(?:\\.\\d+)?[bBsSlLfFdD]?(?!\\w)"));
    rule.format = numberFormat;
    highlightingRules.append(rule);

    posFormat.setForeground(QColor("#AB81CD"));
    rule.pattern = QRegularExpression(QStringLiteral("[~^]?-?\\d+(?:\\s|$)"));
    rule.format  = posFormat;
    highlightingRules.append(rule);

    namespacedIDFormat.setForeground(QColor("#45503B"));
    namespacedIDFormat.setFontWeight(QFont::Bold);
    rule.pattern =
        QRegularExpression(QStringLiteral("\\b[a-z0-9-_]+:[a-z0-9-_/.]+\\b"));
    rule.format = namespacedIDFormat;
    highlightingRules.append(rule);

    auto quoteFmt = QTextCharFormat();
    quoteFmt.setForeground(QColor("#A31621"));

    quoteHighlightRules.insert('\'', quoteFmt);

    commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("^#.*"));
    rule.format  = commentFormat;
    highlightingRules.append(rule);
}

void McfunctionHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
            QRegularExpressionMatchIterator matchIterator =
                rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(),
                          rule.format);
            }
        }
    }

    setCurrentBlockState(0);
}
