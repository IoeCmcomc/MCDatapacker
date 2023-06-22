#include "mcfunctionhighlighter.h"

#include "parsers/command/mcfunctionparser.h"
#include "parsers/command/visitors/nodeformatter.h"
#include "parsers/command/visitors/sourceprinter.h"

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

McfunctionHighlighter::McfunctionHighlighter(QTextDocument *parent,
                                             Command::McfunctionParser *parser)
    : Highlighter(parent), m_parser(parser) {
    setupRules();
}

void McfunctionHighlighter::setupRules() {
    m_singleCommentCharset += QStringLiteral("#");

    const QRegularExpression keywordRegex{ QStringLiteral(
                                               R"((?<=^| )(?>(?>a)(?>dvancement|ttribute)|b(?>an(?>-ip|list|)|ossbar)|cl(?>ear|one)|d(?>ata(?:pack)?|e(?>bug|faultgamemode|op)|ifficulty)|e(?>chant|ffect|x(?>ecute|perience))|f(?>ill|orceload|unction)|g(?>ame(?>mode|rule)|ive)|help|item|jfr|ki(?>ck|ll)|l(?>ist|o(?>cate(?:biome)?|ot))|m(?>e|sg)|op|p(?>ar(?>don(?:-ip)?|ticle)|erf|laysound|ublish)|re(?>cipe|load|placeitem)|s(?>a(?>ve-(?>all|o(?>ff|n))|y)|c(?>hedule|oreboard)|e(?>ed|t(?>block|idletimeout|worldspawn))|p(?>awnpoint|ectate|readplayer)|top(?:sound)?|ummon)|t(?>ag|e(?>am(?:msg)?|l(?>eport|l(?:raw)?))|i(?>me|tle)|m|p|rigger)|w(?>eather|hitelist|orldborder|)|xp)(?= |$))") };
    highlightingRules.append({ keywordRegex, CodePalette::Keyword });

    const QRegularExpression numberRegex{ QStringLiteral(
                                              R"((?<!\w)-?\d+(?:\.\d+)?[bBsSlLfFdD]?(?!\w))") };
    highlightingRules.append({ numberRegex, CodePalette::Number });

    highlightingRules.append({ namespacedIdRegex,
                               CodePalette::ResourceLocation });

    const static QRegularExpression commentRegex{ QStringLiteral(R"(^\s*#.*)") };
    highlightingRules.append({ commentRegex, CodePalette::Comment });
}

void McfunctionHighlighter::highlightBlock(const QString &text) {
    Highlighter::highlightBlock(text);
    if (this->document()) {
        auto *data = static_cast<TextBlockData *>(currentBlockUserData());

        if (!data)
            return;

        const auto &infos = data->namespacedIds();

        auto fmt = m_palette[CodePalette::ResourceLocation];
        for (const auto info: infos) {
            fmt.setFontUnderline(true);
            fmt.setToolTip(info->link);
            setFormat(info->start, info->length, fmt);
        }

        if (!isManualHighlight()) {
            for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
                QRegularExpressionMatchIterator &&matchIterator =
                    rule.pattern.globalMatch(text);
                while (matchIterator.hasNext()) {
                    QRegularExpressionMatch &&match = matchIterator.next();
                    setFormat(match.capturedStart(), match.capturedLength(),
                              m_palette[rule.formatRole]);
                }
            }
        } else {
            for (const auto &range:
                 qAsConst(m_formats.at(m_curChangedBlockIndex))) {
                mergeFormat(range.start, range.length, range.format);
            }
            ++m_curChangedBlockIndex;
        }
    }
}

void McfunctionHighlighter::rehighlightChangedBlocks() {
    const auto &&blocks = changedBlocks();

    if (blocks.isEmpty()) {
        return;
    } else if (blocks.size() == 1) {
        Highlighter::rehighlightBlock(blocks.at(0));
    } else if (blocks.size() == document()->blockCount()) {
        Highlighter::rehighlight();
    } else {
        for (const auto &block: blocks) {
            Highlighter::rehighlightBlock(block);
        }
    }

    m_formats.clear();
    m_curChangedBlockIndex = 0;
}

void McfunctionHighlighter::rehighlightDelayed() {
    Q_ASSERT(m_parser != nullptr);

    const auto &&result      = m_parser->syntaxTree();
    const auto &&resultLines = result->lines();
    const auto &&blocks      = changedBlocks();
    m_formats.resize(blocks.size());
    Command::NodeFormatter formatter(m_palette);

    int i = -1;
    for (auto iter = blocks.cbegin(); iter != blocks.cend(); ++iter) {
        ++i;
        const QString &&lineText   = iter->text();
        auto           *lineResult = resultLines[iter->blockNumber()].get();
        if (lineResult->kind() == Command::ParseNode::Kind::Root) {
            Command::SourcePrinter printer;
            printer.startVisiting(lineResult);
            if (printer.source() != lineText) {
                // qDebug() << lineText;
                qDebug() << printer.source();
            }

            formatter.startVisiting(lineResult);
            m_formats[i] = formatter.formatRanges();
            formatter.reset();
        }
    }

    document()->blockSignals(true);
    document()->documentLayout()->blockSignals(true);
    rehighlightChangedBlocks();
    document()->documentLayout()->blockSignals(false);
    document()->blockSignals(false);
}
