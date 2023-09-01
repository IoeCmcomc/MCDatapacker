#include "codegutter.h"

#include "linenumberarea.h"
#include "problemarea.h"

#include <QPainter>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollBar>

CodeGutter::CodeGutter(CodeEditor *parent) : QWidget(parent),
    m_editor(parent) {
    setFont(m_editor->font());

    /* Allocating the layout on the stack
     * causes children widgets to be arranged only once,
       as the layout will be deleted after the construction. */
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_problemArea = new ProblemArea(this);
    layout->addWidget(m_problemArea, 0, 0);

    m_lineNumberArea = new LineNumberArea(this);
    layout->addWidget(m_lineNumberArea, 0, 1);

    adjustSize();
}

CodeEditor * CodeGutter::editor() const {
    return m_editor;
}

void CodeGutter::updateChildrenGeometries() {
    for (auto *child: findChildren<QWidget *>()) {
        child->adjustSize();
        child->updateGeometry();
    }
}

void CodeGutter::paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.fillRect(rect(), QColor(0, 255, 0, 127));
}
