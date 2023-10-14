#include "mcfunctionparser.h"

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
        constexpr static int typeId =
            MinecraftParser::getTypeEnumId<RootNode>();

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
            } else if ((trimmed[0] == u'$'
                        && m_commandParser.gameVer >= Game::v1_20_2)
                       || state == State::Macro) {
                tree->append(SpanPtr::create(spanText(splitter.getCurrLine()),
                                             true));
                validLineCount++;
                advance(line.length() + 1);
                state =
                    trimmed.endsWith(u'\\') ? State::Macro : State::Command;
            } else {
                Q_ASSERT(state == State::Command);
                NodePtr     command;
                const auto &logicalLine =
                    (m_commandParser.gameVer >= Game::v1_20_2)
                    ? splitter.nextLogicalLine()
                    : splitter.getCurrLine();
//                qDebug() << "logicalLine" << logicalLine;
#ifdef MCFUNCTIONPARSER_USE_CACHE
                CacheKey key{ typeId, logicalLine };
                if (!m_cache.contains(key)
                    || !(command = m_cache[key].lock())) {
#endif
                m_commandParser.setText(logicalLine);
                command = m_commandParser.parse();
#ifdef MCFUNCTIONPARSER_USE_CACHE
                if (command->isValid()) {
                    m_cache.emplace(typeId, std::move(logicalLine),
                                    WeakNodePtr(command));
                    validLineCount++;
                }
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
//                 << "). Hit rate:" << m_cache.stats().hit_rate()
//                 << ". Time elapsed:" << timer.nsecsElapsed() / 1e6 << "ms.";

        //        qDebug() << "Time elapsed:" << timer.nsecsElapsed() / 1e6 << "ms.";

        m_tree = tree;
        m_tree->setSourceMapper(splitter.sourceMapper());
        m_spans = m_commandParser.spans();
        m_cache.setCapacity(validLineCount + 1);
        return m_tree->isValid();
    }
}
