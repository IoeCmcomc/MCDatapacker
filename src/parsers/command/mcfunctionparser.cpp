#include "mcfunctionparser.h"

#include "nodes/macronode.h"
#include "re2c_generated_functions.h"
#include "game.h"

#include <QElapsedTimer>

class LineSplitter {
public:
    LineSplitter(const QString &text) : m_text{text} {
    };

    QStringView getCurrLineView() {
        const int pos = m_physPos;

        if (pos == m_text.length()) {
            m_physPos++;
            m_lineNo++;
            return QStringView();
        }
        const int   index = m_text.indexOf('\n', pos);
        QStringView view{ m_text };

        if (index != -1) {
            m_physPos = index + 1;
            m_lineNo++;
            return view.mid(pos, index - pos);
        } else {
            m_physPos = m_text.length() + 1;
            m_lineNo++;
            return view.mid(pos);
        }
    };

    QString getCurrLine() {
        const auto line = getCurrLineView();

        m_srcMapper.logicalLines += m_lineNo;
        m_logiPos                += line.length() + 1;
        return line.toString();
    }

    QStringView peekCurrLineView() const {
        if (m_physPos == m_text.length()) {
            return QStringView();
        }
        const int   index = m_text.indexOf('\n', m_physPos);
        QStringView view{ m_text };

        if (index != -1) {
            return view.mid(m_physPos, index - m_physPos);
        } else {
            return view.mid(m_physPos);
        }
    };

    bool hasNextLine() const {
        return m_physPos <= m_text.length();
    };

    QString nextLogicalLine() {
        if (hasNextLine()) {
            if (m_physPos > 0 && m_lastLineIsContinuation) {
                m_srcMapper.physicalPositions[m_logiPos] = m_physPos;
                m_srcMapper.logicalPositions[m_physPos]  = m_logiPos;
                m_lastLineIsContinuation                 = false;
            }
            QStringView line = getCurrLineView();
            m_srcMapper.logicalLines += m_lineNo;
            QString logicalLine;
            if (canConcatenate(line)) {
                do {
                    line.chop(1);
                    m_logiPos   += line.length();
                    logicalLine += line;
                    if (!hasNextLine()) {
                        break;
                    }
                    const auto lastLine    = line;
                    const int  lastPhysPos = m_physPos - 2;

                    m_srcMapper.logicalPositions[m_physPos] = m_logiPos;

                    line = getCurrLineView().trimmed();

                    m_srcMapper.backslashMap[m_logiPos] = {
                        lastPhysPos,
                        m_text.mid(lastPhysPos,
                                   line.cbegin() - lastLine.cend()) };
                } while (canConcatenate(line));
                m_lastLineIsContinuation = true;
            }
            m_logiPos   += line.length() + 1;
            logicalLine += line;
            return logicalLine;
        }
        return {};
    };

    bool canConcatenate(QStringView line) {
        return !line.isEmpty() && line.back() == QLatin1Char('\\');
    };

    Command::SourceMapper sourceMapper() const {
        return m_srcMapper;
    }

private:
    Command::SourceMapper m_srcMapper;
    QString m_text;
    int m_physPos                 = 0;
    int m_logiPos                 = 0;
    int m_lineNo                  = -1;
    bool m_lastLineIsContinuation = false;
};

namespace Command {
    McfunctionParser::McfunctionParser() {
    }

    QSharedPointer<FileNode> McfunctionParser::syntaxTree() const {
        return m_tree;
    }

    bool McfunctionParser::parseImpl() {
        constexpr static int cmdTypeId =
            MinecraftParser::getTypeEnumId<RootNode>();
        constexpr static int macroTypeId
            = static_cast<int>(nodeTypeEnum<MacroNode, ParseNode::Kind>);

        const auto &&tree           = QSharedPointer<FileNode>::create();
        const auto &&txt            = text(); // This prevent crash in release build
        int          validLineCount = 0;

        m_commandParser.m_spans = std::move(m_spans);

//        QElapsedTimer timer;
//        timer.start();

        State        state = State::Command;
        LineSplitter splitter{ txt };
        while (splitter.hasNextLine()) {
            const int linePos = pos();
//            qDebug() << "linePos" << linePos << splitter.peekCurrLineView();
            const auto line    = splitter.peekCurrLineView();
            const auto trimmed = line.trimmed();
            if (trimmed.isEmpty() || trimmed[0] == u'#' ||
                state == State::Comment) {
                tree->append(SpanPtr::create(spanText(splitter.getCurrLine()),
                                             true));
                validLineCount++;
                advance(line.length() + 1);
                state =
                    trimmed.endsWith(u'\\') ? State::Comment : State::Command;
            } else {
                const auto &logicalLine =
                    (m_commandParser.gameVer >= Game::v1_20_2)
                        ? splitter.nextLogicalLine()
                        : splitter.getCurrLine();
                if (trimmed[0] == u'$'
                    && m_commandParser.gameVer >= Game::v1_20_2) {
                    NodePtr macro;
#ifdef MCFUNCTIONPARSER_USE_CACHE
                    CacheKey key{ macroTypeId, logicalLine };
                    if (!m_cache.contains(key)
                        || !(macro = m_cache[key].lock())) {
#endif
                    macro = parseMacroLine(logicalLine, linePos);
#ifdef MCFUNCTIONPARSER_USE_CACHE
                    if (macro->isValid()) {
                        m_cache.emplace(macroTypeId, std::move(logicalLine),
                                        WeakNodePtr(macro));
                        validLineCount++;
                    }
                } else {
                    validLineCount++;
                }
#endif
                    tree->append(macro);
                    advance(logicalLine.length() + 1);
                    state = State::Command;
                } else {
                    Q_ASSERT(state == State::Command);
                    NodePtr command;

#ifdef MCFUNCTIONPARSER_USE_CACHE
                    CacheKey key{ cmdTypeId, logicalLine };
                    if (!m_cache.contains(key)
                        || !(command = m_cache[key].lock())) {
#endif
                    m_commandParser.setText(logicalLine);
                    command = m_commandParser.parse();
#ifdef MCFUNCTIONPARSER_USE_CACHE
                    if (command->isValid()) {
                        m_cache.emplace(cmdTypeId, std::move(logicalLine),
                                        WeakNodePtr(command));
                        validLineCount++;
                    }
                } else {
                    validLineCount++;
                }
#endif

                    if (!command->isValid()) {
                        auto errors = m_commandParser.errors();
                        for (int i = 0; i < errors.length(); ++i) {
                            errors[i].pos += linePos;
                            const auto &error = errors[i];
                            if (!m_errors.contains(error)) {
                                m_errors << error;
                            }
                        }
                    }
                    tree->append(std::move(command));
                    advance(logicalLine.length() + 1);
                }
            }
        }

        const auto &&posMapping = splitter.sourceMapper().physicalPositions;
//        qDebug() << posMapping << splitter.sourceMapper().backslashMap;

        if (!posMapping.empty()) {
            for (auto &error: m_errors) {
                const int pos = error.pos;
//                qDebug() << pos << posMapping.cbegin().key();
                if (pos >= posMapping.cbegin().key()) {
                    auto &&nearest = posMapping.upperBound(pos);
                    if (nearest != posMapping.cbegin()) {
                        nearest--;
                    }
//                    qDebug() << "nearest" << nearest.key() << nearest.value();
                    error.pos += nearest.value() - nearest.key();
//                    qDebug() << "error.pos =" << pos << '+' <<
//                        (nearest.value() - nearest.key()) << '=' << error.pos;
                }
                const int   endPos       = pos + error.length - 1;
                const auto &backslashMap = splitter.sourceMapper().backslashMap;
                for (auto it = backslashMap.lowerBound(pos);
                     it != backslashMap.upperBound(endPos); ++it) {
//                    qDebug() << "backslash" << it.key() << it.value();
                    error.length += it.value().trivia.length();
                }
            }
        }

//        const auto &cache = m_commandParser.cache();
//        qInfo() << "Size:" << cache.size() << '/' << cache.capacity()
//                << "Total access:" << cache.stats().total_accesses()
//                << ". Total hit:" << cache.stats().total_hits()
//                << ". Total miss:" << cache.stats().total_misses()
//                << ". Hit rate:" << cache.stats().hit_rate();

//        qDebug() << "Size:" << m_cache.size() << '/' << m_cache.capacity()
//                 << "Total access:" << m_cache.stats().total_accesses()
//                 << "(hit:" << m_cache.stats().total_hits()
//                 << ", miss:" << m_cache.stats().total_misses()
//                 << "). Hit rate:" << m_cache.stats().hit_rate();
//                 << ". Time elapsed:" << timer.nsecsElapsed() / 1e6 << "ms.";

        //        qDebug() << "Time elapsed:" << timer.nsecsElapsed() / 1e6 << "ms.";

//        m_cache.print();

        m_tree = tree;
        m_tree->setSourceMapper(splitter.sourceMapper());
        m_spans = m_commandParser.spans();
        m_cache.setCapacity(validLineCount + 1);
        return m_tree->isValid();
    }

    QSharedPointer<MacroNode> McfunctionParser::parseMacroLine(
        const QString &line, const int linePos) {
        static QStringMatcher matcher{ "$("_QL1 };

        QStringView lineView{ line };
        Parser macroParser{ line };
        int varCount = 0;

        macroParser.skipWs(false);
        macroParser.advance();

        auto subLine = QSharedPointer<MacroNode>::create(line.length());
        int varStart = matcher.indexIn(line);

        while (varStart != -1) {
            varCount++;
            if (const auto span = macroParser.getUntil('$'); !span.isEmpty()) {
                subLine->append(SpanPtr::create(spanText(span), true));
            }
            macroParser.advance(2);
            int varEnd = line.indexOf(')', varStart);

            QSharedPointer<MacroVariableNode> var;
            if (varEnd != -1) {
                const QStringView varKey =
                    lineView.mid(varStart + 2, varEnd - (varStart + 2));
                macroParser.advance(varKey.length());
                // Player names and macro variables have the same charset.
                const auto actualVarKey = re2c::realPlayerName(varKey);
                if (actualVarKey.length() == varKey.length()) {
                    var = QSharedPointer<MacroVariableNode>::create(
                        spanText(varKey), varKey.length() + 3, true);
                } else {
                    var = QSharedPointer<MacroVariableNode>::create(
                        spanText(varKey), varKey.length() + 3, false);
                    reportError(QT_TR_NOOP("Invalid macro variable name '%1'"),
                                { varKey.toString() }, linePos + varStart + 2,
                                varEnd - varStart - 1);
                }
                var->setLeftText(spanText("$("_QL1));
                macroParser.advance(varKey.length());
                var->setRightText(spanText(macroParser.curChar()));
                macroParser.advance();
                subLine->append(std::move(var));
            } else {
                const auto rest = macroParser.getRest();
                var = QSharedPointer<MacroVariableNode>::create(
                    spanText(rest), false);
                var->setLeftText(spanText("$("_QL1));
                macroParser.advance(rest.length());
                reportError(QT_TR_NOOP("Unterminated macro variable"),
                            {}, linePos + varStart, rest.length());
                subLine->append(std::move(var));
                break;
            }
            varStart = matcher.indexIn(line, varEnd + 1);
        }
        if (const auto rest = macroParser.getRest(); !rest.isEmpty()) {
            subLine->append(SpanPtr::create(spanText(rest), true));
        }

        if (varCount == 0) {
            reportError(QT_TR_NOOP("A macro requires at least one variable."),
                        {}, linePos, line.length());
            subLine->setIsValid(false);
        }

        return subLine;
    }
}
