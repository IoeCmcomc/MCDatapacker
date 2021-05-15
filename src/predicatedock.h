#ifndef PREDICATEDOCK_H
#define PREDICATEDOCK_H

#include <QDockWidget>
#include <QHBoxLayout>

namespace Ui {
    class PredicateDock;
}

class PredicateDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit PredicateDock(QWidget *parent = nullptr);
    ~PredicateDock();

protected:
    void changeEvent(QEvent *event) override;

protected slots:
    void onReadBtn();
    void onWriteBtn();

private:
    Ui::PredicateDock *ui;
};

#endif /* PREDICATEDOCK_H */
