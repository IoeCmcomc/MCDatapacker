#include "mcfunctionparser.h"

namespace Command {
    McfunctionParser::McfunctionParser() {
    }

    QSharedPointer<FileNode> McfunctionParser::syntaxTree() const {
        return m_tree;
    }

    bool McfunctionParser::parseImpl() {
        m_tree = QSharedPointer<FileNode>::create();

        const auto &&txt   = text(); // This prevent crash in release build
        const auto &&lines = txt.splitRef(QChar::LineFeed);

        for (const auto &line: lines) {
            const int   linePos = pos();
            const auto &trimmed = line.trimmed();
            if (trimmed.isEmpty() || trimmed[0] == '#') {
                const auto &&span = SpanPtr::create(spanText(line));
                m_tree->append(std::move(span));
            } else {
                m_commandParser.setText(line.toString());
                const auto &&command = m_commandParser.parse();
                if (m_tree->isValid()) {
                    m_tree->setIsValid(command->isValid());
                }
                m_spans |=
                    m_commandParser.parsingResult().spans;
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
                m_tree->append(std::move(command));
            }
            advance(line.length());
            if (curChar() == QChar::LineFeed) {
                advance();
            }
        }
        return m_tree->isValid();
    }
}
