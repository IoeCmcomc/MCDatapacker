#include "stripedscrollbar.h"

#include "codeeditor.h"

#include <QPainter>
#include <QStyleOptionSlider>
#include <QTextBlock>

StripedScrollBar::StripedScrollBar(Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent) {
    if (auto *editor = qobject_cast<CodeEditor *>(parent)) {
        m_editor = editor;
    }
    Q_ASSERT(m_editor);
}

void StripedScrollBar::paintEvent(QPaintEvent *event) {
    QScrollBar::paintEvent(event);

    if (m_editor->document()) {
        QPainter p(this);
        p.drawPixmap(rect(), m_stripes);
    }
}

void StripedScrollBar::resizeEvent(QResizeEvent *event) {
    QScrollBar::resizeEvent(event);
    redrawStripes();
}

void StripedScrollBar::redrawStripes() {
    static const QColor errorColor(255, 0, 0, 100);
    static const QColor warningColor(255, 255, 0, 100);

    m_stripes = QPixmap(size());
    m_stripes.fill(Qt::transparent);
    QPainter p(&m_stripes);
    p.setPen(Qt::NoPen);

    Q_ASSERT(m_editor->document());
    auto     *doc   = m_editor->document();
    const int lines = doc->blockCount();

    QStyleOptionSlider option;
    option.initFrom(this);
    option.orientation = orientation();
    const auto &&r = style()->subControlRect(QStyle::CC_ScrollBar, &option,
                                             QStyle::SC_ScrollBarGroove, this);
    int x, y, w, h;
    r.getRect(&x, &y, &w, &h);

    const auto &problems    = m_editor->m_problems;
    auto       *prevProblem = problems.cbegin();
    auto        block       = doc->firstBlock();

    const auto predicate = [&block](const ProblemInfo &problem) {
                               return block.contains(problem.pos);
                           };

    if (orientation() == Qt::Vertical) {
        for (; block != doc->end(); block = block.next()) {
            const int stripeY = y + (h * block.blockNumber() / lines);
            int       i       = 0;
            for (auto problem =
                     std::find_if(prevProblem, problems.cend(), predicate);
                 (problem != problems.cend()) && block.contains(problem->pos);
                 ++problem) {
                p.setBrush((problem->type == ProblemInfo::Type::Error)
                            ? errorColor : warningColor);
                const int stripeThickness = qBound(1, h / lines, 32);
                p.drawRect(x, stripeY, w, stripeThickness);

                prevProblem = problem;
                ++i;
                if (i >= 3)
                    break;
            }
        }
    }
}
