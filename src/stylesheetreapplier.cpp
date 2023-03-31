#include "stylesheetreapplier.h"

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QPointer>
#include "qevent.h"

bool StyleSheetReapplier::eventFilter(QObject *object, QEvent *event) {
    if ((event->type() == QEvent::StyleChange) && !m_reapplying) {
        QPointer<QWidget> widget = qobject_cast<QWidget *>(object);
        // The widget may be deleted afterward
        QTimer::singleShot(100, widget, [widget, this]{
            if (widget) {
                m_reapplying = true;
                widget->setStyleSheet(widget->styleSheet());
//                qDebug() << "Slylesheet re-applied for" << widget;
                m_reapplying = false;
            } else {
//                qDebug() << "The widget is deleted";
            }
        });
    }
    return QObject::eventFilter(object, event);
}

