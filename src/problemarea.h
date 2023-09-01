#ifndef PROBLEMAREA_H
#define PROBLEMAREA_H

#include <QWidget>
#include <QMultiMap>

class CodeGutter;
struct ProblemInfo;

class ProblemArea : public QWidget {
    Q_OBJECT
public:
    explicit ProblemArea(CodeGutter *parent = nullptr);
    QSize sizeHint() const override;

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QMultiMap<int, const ProblemInfo *> m_problems;
    CodeGutter *m_gutter = nullptr;
};

#endif /* PROBLEMAREA_H */
