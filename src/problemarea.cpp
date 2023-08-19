#include "problemarea.h"

#include "codegutter.h"
#include "codeeditor.h"

#include <QPainter>
#include <QSvgRenderer>
#include <QToolTip>

ProblemArea::ProblemArea(CodeGutter *parent)
    : QWidget(parent), m_gutter(parent) {
}

QSize ProblemArea::sizeHint() const {
    return QSize(fontMetrics().ascent(), 300);
}

bool ProblemArea::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        const auto   *helpEvent = static_cast<QHelpEvent *>(event);
        const QPoint &globalPos = helpEvent->globalPos();
        auto         *editor    = m_gutter->editor();

        const auto &&cursor      = editor->cursorForPosition(helpEvent->pos());
        const int    blockNumber = cursor.blockNumber();

        if (m_problems.contains(blockNumber) && !cursor.atEnd()) {
            const auto &&problems = m_problems.values(blockNumber);
            Q_ASSERT(!problems.isEmpty());
            QString displayText = tr(
                "<b>There be %n problem(s) in line %1:</b><ol>",
                nullptr, problems.size()).arg(blockNumber + 1);

            for (const auto *problem: problems) {
                displayText += QStringLiteral("<li>");
                displayText += problem->message;
                displayText += QStringLiteral("</li>");
            }
            QToolTip::showText(globalPos, displayText, this);
            event->accept();
            return true;
        } else {
            QToolTip::hideText();
            event->ignore();
            return false;
        }
    } else {
        return QWidget::event(event);
    }
}

void ProblemArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(event->rect(), palette().midlight());

    auto        *editor      = m_gutter->editor();
    QTextBlock &&block       = editor->firstVisibleBlock();
    int          blockNumber = block.blockNumber();

    const auto predicate = [&block](const ProblemInfo &problem) {
                               return block.contains(problem.pos);
                           };

    const auto &problems    = editor->m_problems;
    auto       *prevProblem = problems.cbegin();
    m_problems.clear();

    int top =
        qRound(editor->blockBoundingGeometry(block).translated(
                   editor->contentOffset()).top());
    int bottom = top + qRound(editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            bool hasProblem = false;
            bool hasError   = false;
            bool hasWarning = false;
            for (const auto *problem =
                     std::find_if(prevProblem, problems.cend(), predicate);
                 (problem != problems.cend()) && block.contains(problem->pos);
                 ++problem) {
                hasProblem  = true;
                hasError   |= problem->type == ProblemInfo::Type::Error;
                hasWarning |= problem->type == ProblemInfo::Type::Warning;
                prevProblem = problem;
                m_problems.insert(blockNumber, problem);
            }

            if (hasProblem) {
                static QSvgRenderer renderer;
                if (hasError) {
                    if (hasWarning) {
                        renderer.load(QLatin1String(
                                          ":/icon/gutter/error-and-warning.svg"));
                    } else {
                        renderer.load(QLatin1String(
                                          ":/icon/gutter/error.svg"));
                    }
                } else if (hasWarning) {
                    renderer.load(QLatin1String(
                                      ":/icon/gutter/warning.svg"));
                }
                if (hasError || hasWarning) {
                    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
                    QRect areaRect(0, top, width(),
                                   fontMetrics().height());
                    const int margin = areaRect.height() * 15 / 100;
                    areaRect.adjust(margin, margin, -margin, -margin);
                    renderer.render(&painter, areaRect);
                }
            }
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
