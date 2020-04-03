#ifndef PREDICATEDOCK_H
#define PREDICATEDOCK_H

#include <QDockWidget>

namespace Ui {
class PredicateDock;
}

class PredicateDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit PredicateDock(QWidget *parent = nullptr);
    ~PredicateDock();

private:
    Ui::PredicateDock *ui;
};

#endif // PREDICATEDOCK_H
