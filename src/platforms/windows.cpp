#include "windows.h"

#ifdef Q_OS_WIN

#include <QtWin>
#include <QSettings>

#include "dwmapi.h"

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE    20
#endif

#endif

static const QOperatingSystemVersion Windows11 =
{ QOperatingSystemVersion::OSType::Windows, 10, 0, 22000 };

namespace Windows {
    bool isWindows11() {
        return QOperatingSystemVersion::current() >= Windows11;
    }

    void extendFrame(QWidget *w, const QString &className) {
#ifdef Q_OS_WIN
        if (QtWin::isCompositionEnabled()) {
            w->setAttribute(Qt::WA_TranslucentBackground, true);
            w->setAttribute(Qt::WA_NoSystemBackground, false);
            w->setStyleSheet(QStringLiteral(
                                 "%1, QLabel, QTabWidget::tab-bar { background: transparent; }").arg(
                                 className));
            QtWin::extendFrameIntoClientArea(w, -1, -1, -1, -1);
        } else {
            w->setAttribute(Qt::WA_TranslucentBackground, false);
            QtWin::resetExtendedFrame(w);
        }
#endif
    }

    void setDarkFrame(QWidget *w) {
#ifdef Q_OS_WIN
        BOOL value = TRUE;
        ::DwmSetWindowAttribute((HWND)w->winId(), DWMWA_USE_IMMERSIVE_DARK_MODE,
                                &value, sizeof(value));
#endif
    }

    bool isDarkMode() {
#ifdef Q_OS_WIN
        if (QOperatingSystemVersion::current() <
            QOperatingSystemVersion::Windows10) {
            return false;
        }
        QSettings registry(
            R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
            QSettings::NativeFormat);

        return !registry.value(QStringLiteral("AppsUseLightTheme"),
                               true).toBool();
#endif
        return false;
    }

    void setDarkFrameIfDarkMode(QWidget *w) {
#ifdef Q_OS_WIN
        if (isDarkMode())
            setDarkFrame(w);
#endif
    }
}
