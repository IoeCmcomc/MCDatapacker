#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "codeeditor.h"

#include <QWidget>

class CodeGutter;

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeGutter *parent);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    CodeGutter *m_gutter = nullptr;
    QTextCursor txtCursor;
};

#endif /* LINENUMBERAREA_H */
