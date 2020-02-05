#include "stackedwidget.h"

#include <QDebug>

StackedWidget::StackedWidget(QWidget *parent) : QStackedWidget(parent)
{
    connect(this, &QStackedWidget::currentChanged, this, &StackedWidget::onCurrentChanged);
    onCurrentChanged(0);
}

int StackedWidget::addWidget(QWidget *wid) {
    //qDebug() << "addWidget" << wid;
    int i = currentIndex();
    int r = this->QStackedWidget::addWidget(wid);
    wid->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    if(widget(i) != nullptr)
        widget(i)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    adjustSize();
    return r;
    //return QStackedWidget::addWidget(wid);
}

void StackedWidget::onCurrentChanged(int index) {
    //qDebug() << "onCurrentChanged" << index;
    if(widget(lastIndex) != nullptr)
        widget(lastIndex)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    if(widget(index) != nullptr)
        widget(index)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    adjustSize();

    lastIndex = index;
}
