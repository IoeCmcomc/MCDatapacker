#include "problemarea.h"

#include "codegutter.h"
#include "codeeditor.h"

#include <QPainter>
#include <QSvgRenderer>

ProblemArea::ProblemArea(CodeGutter *parent)
    : QWidget(parent), m_gutter(parent) {
}

QSize ProblemArea::sizeHint() const {
    return QSize(fontMetrics().ascent(), 300);
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

    int top =
        qRound(editor->blockBoundingGeometry(block).translated(
                   editor->contentOffset()).top());
    int bottom = top + qRound(editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            bool hasProblem = false;
            bool hasError   = false;
            bool hasWarning = false;
            for (auto problem =
                     std::find_if(prevProblem, problems.cend(), predicate);
                 (problem != problems.cend()) && block.contains(problem->pos);
                 ++problem) {
                hasProblem  = true;
                hasError   |= problem->type == ProblemInfo::Type::Error;
                hasWarning |= problem->type == ProblemInfo::Type::Warning;
                prevProblem = problem;
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
