#include "mcfunctionhighlighter.h"

#include "parsers/command/mcfunctionparser.h"
#include "parsers/command/visitors/nodeformatter.h"
#include "parsers/command/visitors/sourceprinter.h"

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QDebug>


QDebug operator<<(QDebug debug, const QTextLayout::FormatRange &value) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "FormatRange(" << value.start << ", " << value.length <<
        ')';
    return debug;
}

McfunctionHighlighter::McfunctionHighlighter(QTextDocument *parent,
                                             Command::McfunctionParser *parser)
    : Highlighter(parent), m_parser(parser) {
    setHasAdvancedHighlighting(true);
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
    const auto &blocks = changedBlocks();

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

QVector<Command::FormatRanges> McfunctionHighlighter::splitRangesToLines(
    const FormatRanges &ranges, QVector<int> &breakPositions,
    const int offset) {
    QVector<Command::FormatRanges> lines;
    Command::FormatRanges          currentLine;

    for (const auto& range : ranges) {
        int       rangeStart  = range.start;
        int       rangeLength = range.length;
        const int rangeEnd    = rangeStart + rangeLength - 1;

        while (!breakPositions.isEmpty()
               && breakPositions.first() + offset <= rangeEnd) {
            int nextBreak = breakPositions.takeFirst() + offset;
            if (nextBreak < 0) {
                continue;
            }
            if (nextBreak > rangeStart) {
                currentLine.append({ rangeStart, nextBreak - rangeStart,
                                     range.format });
                rangeLength -= nextBreak - rangeStart;
                rangeStart   = nextBreak;
            }
            Q_ASSERT(!currentLine.isEmpty());
            lines << currentLine;
            currentLine.clear();
        }
        currentLine.append({ rangeStart, rangeLength, range.format });
    }

    if (!currentLine.isEmpty()) {
        lines << currentLine;
    }

    return lines;
}

void McfunctionHighlighter::rehighlightDelayed() {
    Q_ASSERT(m_parser != nullptr);

    const auto &&result      = m_parser->syntaxTree();
    const auto &&resultLines = result->lines();
    auto        &blocks      = changedBlocks();
    Q_ASSERT(!blocks.isEmpty());

//    qDebug() << "Initial range:" << blocks.first().blockNumber() << '-' <<
//        blocks.last().blockNumber();

    auto block = blocks.front();
    while (block.isValid()
           && (result->sourceMapper().logicalLinesIndexOf(block.blockNumber())
               == -1)) {
        block = block.previous();
        blocks.prepend(std::move(block));
    }
    block = blocks.back();
    while (block.isValid() && canConcatenate(block.text().trimmed())) {
        block = block.next();
        blocks << std::move(block);
    }

//    qDebug() << "Final range:" << blocks.first().blockNumber() << '-' <<
//        blocks.last().blockNumber();

    m_formats.resize(blocks.size());
    Command::NodeFormatter formatter(m_palette);

    int          i              = -1;
    const auto  &backslashMap   = result->sourceMapper().backslashMap;
    const auto &&breakPosList   = backslashMap.keys();
    auto       &&breakPositions = breakPosList.toVector();
    for (auto iter = blocks.cbegin(); iter != blocks.cend(); ++iter) {
//        qDebug() << "Block" << iter->blockNumber() << "at pos" <<
//            iter->position() << "text:" << iter->text();
        const int lineNumber = result->sourceMapper().logicalLinesIndexOf(
            iter->blockNumber());
        if (lineNumber == -1) {
            continue;
        }
        ++i;
        const QString &&lineText   = iter->text();
        auto           *lineResult = resultLines[lineNumber].get();
        if (lineResult->kind() == Command::ParseNode::Kind::Root) {
            Command::SourcePrinter printer;
            printer.startVisiting(lineResult);
            if (printer.source() != lineText) {
//                qDebug() << lineText;
//                qDebug() << printer.source();
            }

            formatter.startVisiting(lineResult);
            const auto &ranges = formatter.formatRanges();
            if (!backslashMap.empty()) {
                const auto &posMapping =
                    result->sourceMapper().logicalPositions;

                int blockPos = iter->position();
                if (!posMapping.isEmpty()
                    && blockPos >= posMapping.cbegin().key()) {
                    auto &&nearest = posMapping.upperBound(iter->position());
                    if (nearest != posMapping.begin()) {
                        nearest--;
                    }
                    blockPos += nearest.value() - nearest.key();
                }

                auto &&splitedRanges = splitRangesToLines(
                    ranges, breakPositions, -blockPos);

                const bool hasSingleLine = splitedRanges.size() == 1;
                if (hasSingleLine) {
                    m_formats[i] = ranges;
                } else {
                    bool isFirstLine = true;

                    int wsOffset = 0;
                    for (auto &line: splitedRanges) {
                        if (!isFirstLine && (iter != blocks.cend())) {
                            ++iter;

                            auto it = backslashMap.find(
                                result->sourceMapper().logicalPositions.value(
                                    iter->position()));
                            if (it != backslashMap.end()) {
                                wsOffset = it->trivia.length() - 2;
                            }
                        }
                        int firstPos = line.first().start;

                        for (auto &range: line) {
                            if (!isFirstLine) {
                                range.start -= firstPos;
                                range.start += wsOffset;
                            }
                            if (range != line.last()) {
                                range.length += 1;
                            }
                        }
                        if (isFirstLine) {
                            isFirstLine = false;
                        }
                    }
                    for (int j = 0; j < splitedRanges.size(); ++j) {
                        m_formats[i + j] = splitedRanges.at(j);
                    }
                    i += splitedRanges.size() - 1;
                }
            } else {
                m_formats[i] = ranges;
            }
            formatter.reset();
        }
    }

    document()->blockSignals(true);
    document()->documentLayout()->blockSignals(true);
    rehighlightChangedBlocks();
    document()->documentLayout()->blockSignals(false);
    document()->blockSignals(false);
}
