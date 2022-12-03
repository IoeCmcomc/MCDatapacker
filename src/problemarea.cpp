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

bool hasType(const QVector<ProblemInfo> &problems, ProblemInfo::Type type) {
    return std::any_of(problems.cbegin(), problems.cend(),
                       [type](const ProblemInfo &problem){
        return problem.type == type;
    });
}

void ProblemArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(event->rect(), palette().midlight());

    auto        *editor      = m_gutter->editor();
    QTextBlock &&block       = editor->firstVisibleBlock();
    int          blockNumber = block.blockNumber();

    int top =
        qRound(editor->blockBoundingGeometry(block).translated(
                   editor->contentOffset()).top());
    int bottom = top + qRound(editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            if (const auto *data =
                    dynamic_cast<TextBlockData *>(block.userData())) {
                const auto &problems = data->problems();
                if (!problems.isEmpty()) {
                    static QSvgRenderer renderer;
                    bool                hasError =
                        hasType(problems, ProblemInfo::Type::Error);
                    bool hasWarning = hasType(problems,
                                              ProblemInfo::Type::Warning);
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
        }

        block  = block.next();
        top    = bottom;
        bottom = top + qRound(editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
