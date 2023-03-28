#ifndef WINDOWS_H
#define WINDOWS_H

#include <QtGlobal>
#include <QOperatingSystemVersion>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Windows {
    bool isWindows11();
    bool isDarkMode();
    void extendFrame(QWidget *w, const QString &className);
    void setDarkFrame(QWidget *w);
    void setDarkFrameIfDarkMode(QWidget *w);
}

#endif // WINDOWS_H
