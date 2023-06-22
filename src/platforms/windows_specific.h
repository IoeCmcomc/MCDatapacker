#ifndef WINDOWS_SPECIFIC_H
#define WINDOWS_SPECIFIC_H

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

#endif // WINDOWS_SPECIFIC_H
