#include "vieweventfilter.h"

#include <QAbstractItemView>
#include <QContextMenuEvent>
#include <QEvent>

ViewEventFilter::ViewEventFilter(QObject *parent) : QObject(parent) {
}

bool ViewEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ContextMenu) {
        auto *abstractView = qobject_cast<QAbstractItemView*>(obj);
        if (abstractView != nullptr) {
            auto *model     = abstractView->model();
            auto *menuEvent = static_cast<QContextMenuEvent*>(event);
            auto  finalPos  = abstractView->viewport()->
                              mapFromGlobal(abstractView->
                                            mapToGlobal(menuEvent->pos()));
            auto index = abstractView->indexAt(finalPos);
            if (index.isValid()) {
                model->removeRow(index.row());
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}
