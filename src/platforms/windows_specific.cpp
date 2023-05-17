#include "windows_specific.h"

#ifdef Q_OS_WIN

#include "stylesheetreapplier.h"

#include <QtWin>
#include <QSettings>

#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE    20
#endif

#endif

const static QString transparentStyleSheet = QStringLiteral(
    "%1, QLabel, QTabWidget::tab-bar { background: transparent; }");

static const QOperatingSystemVersion Windows11 =
{ QOperatingSystemVersion::OSType::Windows, 10, 0, 22000 };

namespace Windows {
    bool isWindows11() {
        return QOperatingSystemVersion::current() >= Windows11;
    }

    void extendFrame(QWidget *w) {
#ifdef Q_OS_WIN
        if (QtWin::isCompositionEnabled()) {
            w->setAttribute(Qt::WA_TranslucentBackground, true);
            w->setAttribute(Qt::WA_NoSystemBackground, false);
            w->setStyleSheet(transparentStyleSheet.arg(
                                 w->metaObject()->className()));
            QtWin::extendFrameIntoClientArea(w, -1, -1, -1, -1);
            w->installEventFilter(styleSheetReapplier);
        } else {
            w->setAttribute(Qt::WA_TranslucentBackground, false);
            QtWin::resetExtendedFrame(w);
        }
#endif
    }

    void setDarkFrame(QWidget *w, const bool dark) {
#ifdef Q_OS_WIN
        BOOL value = dark;
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
