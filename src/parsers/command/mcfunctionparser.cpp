#include "mcfunctionparser.h"

#include "game.h"

#include <QElapsedTimer>

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

        for (const auto line: lines) {
            const int  linePos = pos();
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
                QString &&lineText = line.toString();
                CacheKey  key{ typeId, lineText };
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
            advance(line.length());
            if (curChar() == QChar::LineFeed) {
                advance();
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

        m_tree  = tree;
        m_spans = m_commandParser.spans();
        m_cache.setCapacity(validLineCount + 1);
        return m_tree->isValid();
    }
}
