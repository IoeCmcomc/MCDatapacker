#ifndef LINESPLITTER_H
#define LINESPLITTER_H

#include "command/nodes/filenode.h"

#include <QStringView>

/*!
    \class LineSplitter
    \brief Lazily splits a QString into lines, with line continuation support.
 */
class LineSplitter {
public:
    LineSplitter(const QString &text);

    QStringView getCurrLineView();
    QString getCurrLine();
    QStringView peekCurrLineView() const;
    bool hasNextLine() const;
    QString nextLogicalLine();
    bool canConcatenate(QStringView line);

    Command::SourceMapper sourceMapper() const;

private:
    Command::SourceMapper m_srcMapper;
    QString m_text;
    int m_physPos                 = 0;
    int m_logiPos                 = 0;
    int m_lineNo                  = -1;
    bool m_lastLineIsContinuation = false;
};

#endif // LINESPLITTER_H
