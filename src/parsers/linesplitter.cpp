#include "linesplitter.h"

LineSplitter::LineSplitter(const QString &text) : m_text{text} {
}

QStringView LineSplitter::getCurrLineView() {
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
}

QString LineSplitter::getCurrLine() {
    const auto line = getCurrLineView();

    m_srcMapper.logicalLines += m_lineNo;
    m_logiPos                += line.length() + 1;
    return line.toString();
}

QStringView LineSplitter::peekCurrLineView() const {
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
}

bool LineSplitter::hasNextLine() const {
    return m_physPos <= m_text.length();
}

QString LineSplitter::nextLogicalLine() {
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
}

bool LineSplitter::canConcatenate(QStringView line) {
    return !line.isEmpty() && line.back() == QLatin1Char('\\');
}

Command::SourceMapper LineSplitter::sourceMapper() const {
    return m_srcMapper;
}
