#include "mcfunctionparser.h"

#include "game.h"

#include <QElapsedTimer>

class LineSplitter {
public:
    LineSplitter(const QString &text) : m_text{text} {
    };

    QStringView nextLineView() {
        const int pos = m_physPos;

        if (pos == m_text.length()) {
            m_physPos++;
            m_lineNo++;
            return QString();
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
            QStringView line = nextLineView();
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

                    line = nextLineView().trimmed();

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
        return !line.isEmpty() && line.endsWith('\\');
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
        const auto &&lines          = QStringView(txt).split(QChar::LineFeed);
        int          validLineCount = 0;

        m_commandParser.m_spans = std::move(m_spans);

        QElapsedTimer timer;
        timer.start();

        LineSplitter splitter{ txt };
        while (splitter.hasNextLine()) {
            const int linePos = pos();
//            const auto line    = splitter.nextLineView();
            const auto line    = splitter.nextLogicalLine();
            const auto trimmed = line.trimmed();
            if (trimmed.isEmpty() || trimmed[0] == u'#') {
                tree->append(SpanPtr::create(spanText(line), true));
                validLineCount++;
            } else if (trimmed[0] == u'$'
                       && m_commandParser.gameVer >= Game::v1_20) {
                tree->append(SpanPtr::create(spanText(line), true));
                validLineCount++;
            } else {
                NodePtr command;
#ifdef MCFUNCTIONPARSER_USE_CACHE
//                QString &&lineText = line.toString();
                const auto &lineText = line;
                CacheKey    key{ typeId, lineText };
                if (!m_cache.contains(key)
                    || !(command = m_cache[key].lock())) {
#endif
                m_commandParser.setText(line);
                command = m_commandParser.parse();
#ifdef MCFUNCTIONPARSER_USE_CACHE
                if (command->isValid()) {
                    m_cache.emplace(typeId, std::move(lineText),
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
            }
            advance(line.length() + 1);
        }

        const auto &&posMapping = splitter.sourceMapper().physicalPositions;

        if (!posMapping.empty()) {
            for (auto &error: m_errors) {
                const int pos = error.pos;
                if (pos >= posMapping.cbegin().key()) {
                    auto &&nearest = posMapping.upperBound(pos);
                    if (nearest != posMapping.cbegin()) {
                        nearest--;
                    }
                    error.pos += nearest.value() - nearest.key();
                }
                const int   endPos       = pos + error.length - 1;
                const auto &backslashMap = splitter.sourceMapper().backslashMap;
                for (auto it = backslashMap.lowerBound(pos);
                     it != backslashMap.upperBound(endPos); ++it) {
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
