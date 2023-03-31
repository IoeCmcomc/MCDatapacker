#ifndef WINDOWS_H
#define WINDOWS_H

#include <QOperatingSystemVersion>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Windows {
    bool isWindows11();
    bool isDarkMode();
    void extendFrame(QWidget *w);
    void setDarkFrame(QWidget *w, const bool dark = true);
    void setDarkFrameIfDarkMode(QWidget *w);
}

#endif // WINDOWS_H
