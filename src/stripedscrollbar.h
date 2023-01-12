#ifndef STRIPEDSCROLLBAR_H
#define STRIPEDSCROLLBAR_H

#include <QScrollBar>

class CodeEditor;

class StripedScrollBar : public QScrollBar {
    Q_OBJECT
public:
    StripedScrollBar(Qt::Orientation orientation, QWidget *parent = nullptr);

    void redrawStripes();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    CodeEditor *m_editor = nullptr;
    QPixmap m_stripes;
};

#endif /* STRIPEDSCROLLBAR_H */
