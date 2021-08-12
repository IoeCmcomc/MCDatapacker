#include "stripedscrollbar.h"

#include "codeeditor.h"

#include <QPainter>
#include <QStyleOptionSlider>

StripedScrollBar::StripedScrollBar(Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent) {
    if (auto *editor = qobject_cast<CodeEditor*>(parent)) {
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
    static const QColor errorColor(255, 0, 0, 127);
    static const QColor warningColor(255, 255, 0, 127);

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

    if (orientation() == Qt::Vertical) {
        for (auto it = doc->firstBlock(); it != doc->end(); it = it.next()) {
            const int stripeY = y + (h * it.blockNumber() / lines);
            if (const auto *data =
                    dynamic_cast<TextBlockData *>(it.userData())) {
                if (const auto &&problem = data->problem(); problem) {
                    p.setBrush(
                        (problem->type == ProblemInfo::Type::Error)
                            ? errorColor : warningColor);
                    const int stripeThickness = qMin(qMax((h / lines), 1), 32);
                    p.drawRect(x, stripeY, w, stripeThickness);
                }
            }
        }
    }
}
