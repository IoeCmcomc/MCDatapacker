#ifndef WINDOWS_H
#define WINDOWS_H

#include <QObject>
#include <QEvent>
#include <QOperatingSystemVersion>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Windows {
    class StyleSheetReapplier : public QObject {
        bool eventFilter(QObject *object, QEvent *event);
        bool m_updated = false;
    };
    Q_GLOBAL_STATIC(StyleSheetReapplier, styleSheetReapplier)

    bool isWindows11();
    bool isDarkMode();
    void extendFrame(QWidget *w);
    void setDarkFrame(QWidget *w, const bool dark = true);
    void setDarkFrameIfDarkMode(QWidget *w);
}

#endif // WINDOWS_H
