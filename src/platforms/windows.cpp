#include "windows.h"

void extendFrameOnWindows(QWidget *w, const QString &className) {
#ifdef Q_OS_WIN
    if (QtWin::isCompositionEnabled()) {
        w->setAttribute(Qt::WA_TranslucentBackground, true);
        w->setAttribute(Qt::WA_NoSystemBackground, false);
        w->setStyleSheet(QStringLiteral("%1, QLabel, QTabWidget::tab-bar { background: transparent; }").arg(className));
        QtWin::extendFrameIntoClientArea(w, -1, -1, -1, -1);
    } else {
        w->setAttribute(Qt::WA_TranslucentBackground, false);
        QtWin::resetExtendedFrame(w);
    }
#endif
}
