#ifndef VIEWEVENTFILTER_H
#define VIEWEVENTFILTER_H

#include <QObject>

class ViewEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit ViewEventFilter(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif /* VIEWEVENTFILTER_H */
