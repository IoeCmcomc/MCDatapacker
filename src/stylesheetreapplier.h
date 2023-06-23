#ifndef STYLESHEETREAPPLIER_H
#define STYLESHEETREAPPLIER_H

#include <QObject>

class StyleSheetReapplier : public QObject {
    bool eventFilter(QObject *object, QEvent *event);
    bool m_reapplying = false;
};

Q_GLOBAL_STATIC(StyleSheetReapplier, styleSheetReapplier)

#endif // STYLESHEETREAPPLIER_H
