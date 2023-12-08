#include "windows_specific.h"

#ifdef Q_OS_WIN

#include "stylesheetreapplier.h"

#include <QtWin>
#include <QSettings>
#include <QDebug>

#include <dwmapi.h>

#define DWMWA_MICA_EFFECT                1029
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE    20
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE        38
#endif
#ifndef DWMSBT_MAINWINDOW
#define DWMSBT_MAINWINDOW                2
#endif
#ifndef DWMSBT_TABBEDWINDOW
#define DWMSBT_TABBEDWINDOW              4
#endif


#else
#include <QString>
#endif

const static QString transparentStyleSheet = QStringLiteral(
    "%1, QLabel, QTabWidget::tab-bar { background: transparent; }");

static const QOperatingSystemVersion Windows11 =
{ QOperatingSystemVersion::OSType::Windows, 10, 0, 22000 };
static const QOperatingSystemVersion Windows11_22523 =
{ QOperatingSystemVersion::OSType::Windows, 10, 0, 22523 };

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

    // Adapted from https://github.com/littlewhitecloud/win32mica/blob/main/win32mica/win32mica.cpp
    void applyMicaEffectToFrame(QWidget *w, const bool micaAlt) {
#ifdef Q_OS_WIN
        if (!QtWin::isCompositionEnabled()
            || (QOperatingSystemVersion::current() < Windows11)) {
            return;
        }
        const HWND hwnd = (HWND)w->winId();
        const bool flag = QOperatingSystemVersion::current() >= Windows11_22523;
        // Set the mica value
        const int value = flag ?
                          (micaAlt ? DWMSBT_TABBEDWINDOW : DWMSBT_MAINWINDOW)
                          : (micaAlt ? 0x04 : 0x01);
        // Set the mica entry
        const int entry = flag ? DWMWA_SYSTEMBACKDROP_TYPE : DWMWA_MICA_EFFECT;

        // Set the window's backdrop
        ::DwmSetWindowAttribute(hwnd, entry, &value, sizeof(int));
#endif
    }

    void applyGlassEffectToFrame(QWidget *w, const bool useMicaAlt) {
#ifdef Q_OS_WIN
        extendFrame(w);
        applyMicaEffectToFrame(w, useMicaAlt);
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
