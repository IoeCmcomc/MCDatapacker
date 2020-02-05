#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>

class StackedWidget : public QStackedWidget
{
public:
    StackedWidget(QWidget *parent = nullptr);

    int addWidget(QWidget *widget);

private:
    int lastIndex = 0;

    void onCurrentChanged(int index);
};

#endif // STACKEDWIDGET_H
