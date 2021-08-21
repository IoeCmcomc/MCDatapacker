#ifndef PROBLEMAREA_H
#define PROBLEMAREA_H

#include <QWidget>

class CodeGutter;

class ProblemArea : public QWidget
{
    Q_OBJECT
public:
    explicit ProblemArea(CodeGutter *parent = nullptr);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeGutter *m_gutter = nullptr;
};

#endif /* PROBLEMAREA_H */
